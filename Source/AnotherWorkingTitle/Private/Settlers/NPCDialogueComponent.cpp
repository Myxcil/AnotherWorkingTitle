// (c) 2025 MK

#include "Settlers/NPCDialogueComponent.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UNPCDialogueComponent::UNPCDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
	{
		// TODO: broadcast error locally.
		return;
	}

	FDialogueRequest Req;
	Req.Messages = BuildMessagesForRequest(PlayerText);
	
	const FGuid RequestId = Service->EnqueueRequest(Req);
	OwnedRequestIds.Add(RequestId);

	History.Add({ EChatTemplateRole::User, PlayerText });
	PruneHistoryIfNeeded();
	
	bFirstRequest = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UNPCDialogueComponent::CancelAllRequests()
{
	if (UDialogueLLMService* Service = GetService())
	{
		Service->CancelAll();
	}
	OwnedRequestIds.Reset();
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
TArray<FDialogueMessage> UNPCDialogueComponent::BuildMessagesForRequest(const FString& PlayerText) const
{
	TArray<FDialogueMessage> Out;

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
