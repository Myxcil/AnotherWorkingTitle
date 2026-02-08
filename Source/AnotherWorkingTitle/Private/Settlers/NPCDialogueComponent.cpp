// (c) 2025 MK

#include "Settlers/NPCDialogueComponent.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AI/GOAPAgentComponent.h"
#include "Settlers/NeedsComponent.h"
#include "Settlers/SocialComponent.h"

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
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::OnError(const FGuid& RequestId, const FString& ErrorText)
{
	if (OwnedRequestIds.Contains(RequestId))
	{
		OwnedRequestIds.Remove(RequestId);
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
	return Result;
}
