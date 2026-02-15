// (c) 2025 MK

#include "Settlers/NPCDialogueComponent.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AI/GOAPAgentComponent.h"
#include "Settlers/NeedsComponent.h"
#include "Settlers/SocialComponent.h"
#include "Settlers/TtsPiperStreamComponent.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UNPCDialogueComponent::UNPCDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!GlobalPromptAsset)
	{
		UE_LOG(LogLLM, Warning, TEXT("No prompt asset assigned to %s"), *GetPathName());
		DestroyComponent();
		return;
	}

	const AActor* Owner = GetOwner();
	check(Owner);
	
	Social = Owner->GetComponentByClass<USocialComponent>();
	check(Social);
	Needs = Owner->GetComponentByClass<UNeedsComponent>();
	check(Needs);
	Agent = Owner->GetComponentByClass<UGOAPAgentComponent>();
	check(Agent);

	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	check(PlayerPawn);
	PlayerSocial = PlayerPawn->GetComponentByClass<USocialComponent>();
	check(PlayerSocial);

	Tts = Owner->GetComponentByClass<UTtsPiperStreamComponent>();
	if (Tts)
	{
		Tts->OnTtsFinished.AddDynamic(this, &UNPCDialogueComponent::HandleTtsFinished);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bInDialog)
	{
		OnEndDialog();
	}
	
	Super::EndPlay(EndPlayReason);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UDialogueLLMService* UNPCDialogueComponent::GetService() const
{
	if (!GetWorld()) 
		return nullptr;
	
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		return GI->GetSubsystem<UDialogueLLMService>();
	}
	
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::OnBeginDialog()
{
	check(!bInDialog);
	bInDialog = true;
	
	OwnedRequestIds.Reset();
	bFirstRequest = true;
	
	CachedSnapshot = FWorldSnapshot();

	if (UDialogueLLMService* Service = GetService())
	{
		Service->SetListener(this);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::OnEndDialog()
{
	check(bInDialog);
	bInDialog = false;
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(IdleFlushTimer);
	}

	ActiveSpeechRequestId.Invalidate();
	AssistantBuffer.Reset();
	SegmentQueue.Reset();
	bReceivedAnyToken = false;
	bTtsBusy = false;

	if (Tts)
	{
		Tts->Stop();
	}
	
	if (UDialogueLLMService* Service = GetService())
	{
		Service->SetListener(nullptr);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::SendPlayerLine(const FString& PlayerText)
{
	if (PlayerText.IsEmpty())
		return;
	
	UDialogueLLMService* Service = GetService();
	if (!Service || !Service->IsReady())
		return;

	FDialogueRequest Req;
	Req.Messages = BuildMessagesForRequest(PlayerText);
	
	const FGuid RequestId = Service->EnqueueRequest(Req);
	OwnedRequestIds.Add(RequestId);

	History.Add({ EChatTemplateRole::User, PlayerText });
	PruneHistoryIfNeeded();
	
	bFirstRequest = false;
	
	// Prepare TTS streaming for this request
	ActiveSpeechRequestId = RequestId;
	bReceivedAnyToken = false;
	AssistantBuffer.Reset();
	SegmentQueue.Reset();
	bTtsBusy = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(IdleFlushTimer);
	}

	if (Tts && bSpeakNPCResponse)
	{
		Tts->Stop(); // interrupt any previous NPC speech; global single-speaker is enforced in TTS component
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::OnTokenGenerated(const FGuid& RequestId, const FString& TokenOrChunk)
{
	if (!bInDialog) 
		return;
	
	if (!OwnedRequestIds.Contains(RequestId)) 
		return;

	if (!bSpeakNPCResponse || !Tts) 
		return;
	
	if (!ActiveSpeechRequestId.IsValid() || RequestId != ActiveSpeechRequestId) 
		return;

	bReceivedAnyToken = true;
	FeedAssistantDelta(RequestId, TokenOrChunk, /*bFinal*/ false);

	// restart idle flush timer (prevents waiting forever for punctuation)
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(IdleFlushTimer);
		GetWorld()->GetTimerManager().SetTimer(
			IdleFlushTimer,
			this,
			&UNPCDialogueComponent::FlushIdle,
			TTS_IdleFlushSeconds,
			false
		);
	}

	TrySpeakNext();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::OnResponseGenerated(const FGuid& RequestId, const FString& FullText)
{
	if (OwnedRequestIds.Contains(RequestId))
	{
		OwnedRequestIds.Remove(RequestId);
		
		History.Add({ EChatTemplateRole::Assistant, FullText });
		PruneHistoryIfNeeded();
		
		OnResponse.Broadcast(RequestId, FullText);

		if (bSpeakNPCResponse && Tts && ActiveSpeechRequestId.IsValid() && RequestId == ActiveSpeechRequestId)
		{
			// If we got no streaming tokens, chunk the full response now.
			if (!bReceivedAnyToken)
			{
				FeedAssistantDelta(RequestId, FullText, /*bFinal*/ true);
				TrySpeakNext();
			}
			else
			{
				// We were streaming; force-flush remainder
				FeedAssistantDelta(RequestId, TEXT(""), /*bFinal*/ true);
				TrySpeakNext();
			}

			if (GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(IdleFlushTimer);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::OnError(const FGuid& RequestId, const FString& ErrorText)
{
	if (OwnedRequestIds.Contains(RequestId))
	{
		OwnedRequestIds.Remove(RequestId);

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(IdleFlushTimer);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
TArray<FDialogueMessage> UNPCDialogueComponent::BuildMessagesForRequest(const FString& PlayerText)
{
	TArray<FDialogueMessage> Out;

	if (bFirstRequest)
	{
		FString SetupContent;
		SetupContent.Append(GlobalPromptAsset->Rules);
		SetupContent.AppendChar('\n');
		SetupContent.Append(GlobalPromptAsset->Lore);
		SetupContent.AppendChar('\n');
		SetupContent.Append(Persona);
		SetupContent.AppendChar('\n');
		
		FDialogueMessage& SetupPrompt = Out.AddDefaulted_GetRef();
		SetupPrompt.Role = EChatTemplateRole::System;
		SetupPrompt.Content = SetupContent;
	}
	
	FWorldSnapshot WorldSnapshot;
	TakeWorldSnapshot(WorldSnapshot);

	const FString WorldMessage = GenerateWorldSnapshotMessage(WorldSnapshot);
	if (!WorldMessage.IsEmpty())
	{
		FDialogueMessage& WorldState = Out.AddDefaulted_GetRef();
		WorldState.Role = EChatTemplateRole::System;
		WorldState.Content = WorldMessage;
		
		CachedSnapshot = WorldSnapshot;
	}
	
	if (bFirstRequest)
	{
		Out.Append(History);
	}
	
	FDialogueMessage& UserLine = Out.AddDefaulted_GetRef();
	UserLine.Role = EChatTemplateRole::User;
	UserLine.Content = PlayerText;
	
	return Out;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::PruneHistoryIfNeeded()
{
	// TODO: Keep last MaxHistoryMessages messages.
	while (History.Num() > MaxHistoryMessages)
	{
		History.RemoveAt(0);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::TakeWorldSnapshot(FWorldSnapshot& Snapshot) const
{
	Social->QueryEmotionalState(Snapshot.Mood);
	Snapshot.Mood.TrimStartAndEndInline();
	
	Social->QueryRelationship(Snapshot.Relationship, PlayerSocial);
	Snapshot.Relationship.TrimStartAndEndInline();
	
	Needs->QueryState(Snapshot.Need);
	Snapshot.Need.TrimStartAndEndInline();
	
	Agent->QueryState(Snapshot.Goal);
	Snapshot.Goal.TrimStartAndEndInline();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString UNPCDialogueComponent::GenerateWorldSnapshotMessage(const FWorldSnapshot& Snapshot) const
{
	FString Result;
	if (!Snapshot.Need.IsEmpty() && Snapshot.Need != CachedSnapshot.Need)
	{
		Result.Appendf(TEXT("- Need: %s\n"), *Snapshot.Need);
	}
	if (!Snapshot.Mood.IsEmpty() && Snapshot.Mood != CachedSnapshot.Mood)
	{
		Result.Appendf(TEXT("- Mood: %s\n"), *Snapshot.Mood);
	}
	if (!Snapshot.Relationship.IsEmpty() && Snapshot.Relationship != CachedSnapshot.Relationship)
	{
		Result.Appendf(TEXT("- Toward player: %s\n"), *Snapshot.Relationship);
	}
	if (!Snapshot.Goal.IsEmpty() && Snapshot.Goal != CachedSnapshot.Goal)
	{
		Result.Appendf(TEXT("- Task: %s\n"), *Snapshot.Goal);
	}
	if (!Result.IsEmpty())
	{
		Result.InsertAt(0, TEXT("WORLD STATE:\n"));
	}
	return Result;
}

void UNPCDialogueComponent::HandleTtsFinished(const FPiperTtsResult& Result)
{
	// Only continue if we're still in dialog and speaking is enabled.
	if (!bInDialog || !bSpeakNPCResponse || !Tts)
	{
		bTtsBusy = false;
		return;
	}

	bTtsBusy = false;
	TrySpeakNext();
}

void UNPCDialogueComponent::FlushIdle()
{
    // emit a chunk even without punctuation if we have enough buffer
    EnqueueSegmentsFromBuffer(/*bForceFlush*/ false);
    TrySpeakNext();
}

void UNPCDialogueComponent::FeedAssistantDelta(const FGuid& RequestId, const FString& Delta, bool bFinal)
{
    if (!ActiveSpeechRequestId.IsValid() || RequestId != ActiveSpeechRequestId)
        return;

    if (!Delta.IsEmpty())
    {
        AssistantBuffer += Delta;
    }

    EnqueueSegmentsFromBuffer(/*bForceFlush*/ bFinal);

    if (bFinal)
    {
        // Force flush any leftover text
        EnqueueSegmentsFromBuffer(/*bForceFlush*/ true);
    }
}

void UNPCDialogueComponent::TrySpeakNext()
{
    if (!bSpeakNPCResponse || !Tts)
    	return;
	
    if (bTtsBusy) 
    	return;
	
    if (SegmentQueue.Num() == 0) 
    	return;

    FString Next = SegmentQueue[0];
    SegmentQueue.RemoveAt(0);

    Next.TrimStartAndEndInline();
    if (Next.IsEmpty())
    {
        TrySpeakNext();
        return;
    }

    bTtsBusy = true;
    Tts->Speak(Next);
}

bool UNPCDialogueComponent::IsSentenceEnd(TCHAR C)
{
    return C == TEXT('.') || C == TEXT('?') || C == TEXT('!');
}

bool UNPCDialogueComponent::IsSoftBreak(TCHAR C)
{
    return C == TEXT(';') || C == TEXT(':') || C == TEXT('\n') || C == TEXT('\r') || C == TEXT('\t') || (C == TEXT(',') );
}

bool UNPCDialogueComponent::IsWhitespace(const TCHAR C)
{
	return C == TEXT(' ') || C == TEXT('\n') || C == TEXT('\r') || C == TEXT('\t');
}

int32 UNPCDialogueComponent::FindSplitIndex(bool bForceFlush) const
{
    const int32 BufferLen = AssistantBuffer.Len();
    if (BufferLen == 0) 
    	return -1;

    const int32 SearchEnd = FMath::Min(BufferLen, TTS_MaxChars);

    auto IsBoundaryOK = [&](const int32 I) -> bool
    {
        if (I < 0 || I >= SearchEnd) 
        	return false;
    	
        const TCHAR C = AssistantBuffer[I];

        const bool bHard = IsSentenceEnd(C);
        const bool bSoft = !bHard && IsSoftBreak(C);

        if (!bHard && !bSoft) 
        	return false;

        // require whitespace/end after boundary if possible (avoid splitting "Mr.Smith")
        if (I + 1 < BufferLen)
        {
            const TCHAR NextChar = AssistantBuffer[I + 1];
            if (!(NextChar == TEXT(' ') || NextChar == TEXT('\n') || NextChar == TEXT('\r') || NextChar == TEXT('\t')))
            {
                return false;
            }
        }
        return true;
    };

    if (BufferLen >= TTS_MinChars)
    {
	    int32 BestScore = INT_MAX;
	    int32 Best = -1;
	    for (int32 I = 0; I < SearchEnd; ++I)
        {
            if (!IsBoundaryOK(I)) 
            	continue;
        	
            const int32 Candidate = I + 1;
            if (Candidate < TTS_MinChars)
            	continue;

            const int32 Score = FMath::Abs(Candidate - TTS_TargetChars);
            if (Score < BestScore)
            {
                BestScore = Score;
                Best = Candidate;
            }
        }
        if (Best > 0) 
        	return Best;
    }

    // Hard cut if buffer too long: split on last whitespace before MaxChars
    if (BufferLen >= TTS_MaxChars)
    {
        for (int32 I = SearchEnd - 1; I > 0; --I)
        {
            const TCHAR C = AssistantBuffer[I];
            if (IsWhitespace(C))
            {
                return I + 1;
            }
        }
        return SearchEnd;
    }

    if (bForceFlush)
    {
        return BufferLen;
    }

    return -1;
}

void UNPCDialogueComponent::EnqueueSegmentsFromBuffer(bool bForceFlush)
{
	int32 MaxRuns = 100000;
    while (--MaxRuns > 0)
    {
        const int32 Split = FindSplitIndex(bForceFlush);
        if (Split <= 0)
        	break;

        FString Chunk = AssistantBuffer.Left(Split);
        AssistantBuffer = AssistantBuffer.Mid(Split);

        Chunk.TrimStartAndEndInline();
        AssistantBuffer.TrimStartInline();

        if (!Chunk.IsEmpty())
        {
            SegmentQueue.Add(Chunk);
        }
    }
	check(MaxRuns > 0);
}
