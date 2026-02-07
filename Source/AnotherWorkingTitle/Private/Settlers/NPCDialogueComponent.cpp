// (c) 2024 by Crenetic GmbH Studios

#include "Settlers/NPCDialogueComponent.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UNPCDialogueComponent::UNPCDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UDialogueLLMService* Service = GetService())
	{
		Service->OnLLMToken.AddDynamic(this, &UNPCDialogueComponent::HandleServiceToken);
		Service->OnLLMResponse.AddDynamic(this, &UNPCDialogueComponent::HandleServiceResponse);
		Service->OnLLMError.AddDynamic(this, &UNPCDialogueComponent::HandleServiceError);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UDialogueLLMService* UNPCDialogueComponent::GetService() const
{
	if (!GetWorld()) return nullptr;
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		return GI->GetSubsystem<UDialogueLLMService>();
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::OnBeginDialog()
{
	History.Reset();
	OwnedRequestIds.Reset();
	if (UDialogueLLMService* Service = GetService())
	{
		Service->Clear();
	}	
	bFirstRequest = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FGuid UNPCDialogueComponent::SendPlayerLine(const FString& PlayerText, const FDialogueContextDescriptor& Context)
{
	if (PlayerText.IsEmpty())
		return FGuid();
	
	UDialogueLLMService* Service = GetService();
	if (!Service || !Service->IsReady())
	{
		// TODO: broadcast error locally.
		return FGuid();
	}

	FDialogueRequest Req;
	Req.Owner = this;
	Req.DebugName = GetOwner() ? GetOwner()->GetName() : TEXT("NPCDialogue");
	Req.Messages = BuildMessagesForRequest(PlayerText, Context);

	const FGuid RequestId = Service->EnqueueRequest(Req);
	OwnedRequestIds.Add(RequestId);

	History.Add({ EChatTemplateRole::User, PlayerText });
	
	PruneHistoryIfNeeded();
	
	bFirstRequest = false;
	
	return RequestId;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::CancelAllRequests()
{
	if (UDialogueLLMService* Service = GetService())
	{
		Service->CancelAllForOwner(this);
	}
	OwnedRequestIds.Reset();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
TArray<FDialogueMessage> UNPCDialogueComponent::BuildMessagesForRequest(const FString& PlayerText, const FDialogueContextDescriptor& Context) const
{
	TArray<FDialogueMessage> Out;

	FString Sys;
	if (bFirstRequest)
	{
		Sys.Append(TEXT("You are an NPC in a small survival colony. Speak as this NPC, in natural English banter.\n"));
		Sys.Append(TEXT("Default to 1-2 sentences unless the player explicitly asks for more detail.\n"));
		Sys.Append(TEXT("\nNPC persona:\n"));
		Sys.Append(PersonaText);
		Sys.AppendChar('\n');
	}
	
	if (!Context.MoodLabel.IsEmpty() || !Context.RelationshipLabel.IsEmpty())
	{
		if (bFirstRequest)
		{
			Sys.Append(TEXT("\nCurrent state (use as guidance for tone and behavior):"));
		}
		else
		{
			Sys.Append(TEXT("\nState update:"));
		}
	
		if (!Context.MoodLabel.IsEmpty())
		{
			Sys.Append(TEXT("\nMood="));
			Sys.Append(Context.MoodLabel);
		}
		if (!Context.RelationshipLabel.IsEmpty())
		{
			Sys.Append(TEXT("\nRelationship="));
			Sys.Append(Context.RelationshipLabel);
		}
	}
	
	Sys.AppendChar('\n');
	if (bFirstRequest)
	{
		for (const FString& Fact : Context.Facts)
		{
			Sys.Append(Fact);
			Sys.AppendChar('\n');
		}
		Sys.Append(Rules);
	}
	Out.Add({ EChatTemplateRole::System, Sys });
		
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
void UNPCDialogueComponent::HandleServiceToken(FGuid RequestId, const FString& TokenOrChunk)
{
	// TODO: if RequestId belongs to OwnedRequestIds => broadcast OnToken
	if (OwnedRequestIds.Contains(RequestId))
	{
		OnToken.Broadcast(RequestId, TokenOrChunk);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::HandleServiceResponse(FGuid RequestId, const FString& FullText)
{
	// TODO: if owned => append assistant message to History; broadcast OnResponse
	if (OwnedRequestIds.Contains(RequestId))
	{
		OwnedRequestIds.Remove(RequestId);
		History.Add({ EChatTemplateRole::Assistant, FullText });
		PruneHistoryIfNeeded();
		OnResponse.Broadcast(RequestId, FullText);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::HandleServiceError(FGuid RequestId, const FString& ErrorText)
{
	// TODO: if owned => broadcast OnError
	if (OwnedRequestIds.Contains(RequestId))
	{
		OwnedRequestIds.Remove(RequestId);
		OnError.Broadcast(RequestId, ErrorText);
	}
}
