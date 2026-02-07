// (c) 2024 by Crenetic GmbH Studios

#include "AI/DialogueLLMService.h"

#include "LlamaComponent.h"

DEFINE_LOG_CATEGORY(LogLLM);

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// TODO: nothing by default. Service is inert until RegisterLlamaComponent.
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::Deinitialize()
{
	if (LlamaComponent)
	{
		LlamaComponent->OnModelLoaded.Clear();
		LlamaComponent->OnError.Clear();
		LlamaComponent->OnTokenGenerated.Clear();
		LlamaComponent->OnPartialGenerated.Clear();
		LlamaComponent->OnResponseGenerated.Clear();
	}
	
	LlamaComponent = nullptr;
	PendingRequests.Reset();
	bModelReady = false;
	Super::Deinitialize();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::RegisterLlamaComponent(ULlamaComponent* InLlamaComponent)
{
	LlamaComponent = InLlamaComponent;
	if (LlamaComponent)
	{
		LlamaComponent->OnModelLoaded.AddUniqueDynamic(this, &ThisClass::HandleModelLoaded);
		LlamaComponent->OnError.AddUniqueDynamic(this, &ThisClass::HandleError);
		//LlamaComponent->OnTokenGenerated.AddUniqueDynamic(this, &ThisClass::HandleNewToken);
		//LlamaComponent->OnPartialGenerated.AddUniqueDynamic(this, &ThisClass::HandlePartialGenerated);
		LlamaComponent->OnResponseGenerated.AddUniqueDynamic(this, &ThisClass::HandleResponseGenerated);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UDialogueLLMService::IsReady() const
{
	return bModelReady && IsValid(LlamaComponent);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FGuid UDialogueLLMService::EnqueueRequest(FDialogueRequest& Request)
{
	Request.RequestId = FGuid::NewGuid();
	PendingRequests.Add(Request);
	TryStartNextRequest();
	return Request.RequestId;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::CancelRequest(const FGuid& RequestId)
{
	if (ActiveRequestId == RequestId)
	{
		if (LlamaComponent)
		{
			LlamaComponent->StopGeneration();
		}
		ActiveRequestId.Invalidate();
		ActiveOwner = nullptr;

		TryStartNextRequest();
	}
	else
	{
		const int32 Index = PendingRequests.IndexOfByPredicate([&](const FDialogueRequest& Request)
		{
			return Request.RequestId == RequestId;
		});
		if (Index != INDEX_NONE)
		{
			PendingRequests.RemoveAt(Index);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::CancelAllForOwner(UObject* Owner)
{
	PendingRequests.RemoveAllSwap([&](const FDialogueRequest& Request)
	{
		return Request.Owner == Owner;	
	});
	if (ActiveOwner == Owner)
	{
		ActiveRequestId.Invalidate();
		if (LlamaComponent)
		{
			LlamaComponent->StopGeneration();
		}
	}
	TryStartNextRequest();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::Clear()
{
	if (LlamaComponent)
	{
		LlamaComponent->StopGeneration();
		LlamaComponent->ResetContextHistory();
	}
	
	PendingRequests.Reset();
	
	ActiveOwner.Reset();
	ActiveRequestId.Invalidate();
	ActiveBuffer.Reset();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandleModelLoaded(const FString& ModelName)
{
	bModelReady = true;
	OnLLMReady.Broadcast(true);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandleNewToken(const FString& Token)
{
	const bool bFirstToken = ActiveBuffer.IsEmpty();
	ActiveBuffer.Append(Token);
	if (bFirstToken)
	{
		SanitizeInPlace(ActiveBuffer);
	}
	OnLLMToken.Broadcast(ActiveRequestId, ActiveBuffer);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandlePartialGenerated(const FString& Chunk)
{
	const bool bFirstToken = ActiveBuffer.IsEmpty();
	ActiveBuffer.Append(Chunk);
	if (bFirstToken)
	{
		SanitizeInPlace(ActiveBuffer);
	}
	OnLLMToken.Broadcast(ActiveRequestId, ActiveBuffer);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandleResponseGenerated(const FString& FullText)
{
	UE_LOG(LogLLM, Log, TEXT("HandleResponseGenerated: %s"), *FullText);

	ActiveBuffer = FullText;
	SanitizeInPlace(ActiveBuffer);
	OnLLMResponse.Broadcast(ActiveRequestId, ActiveBuffer);
	
	ActiveRequestId.Invalidate();
	
	TryStartNextRequest();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandleError(const FString& ErrorMessage, int32 ErrorCode)
{
	UE_LOG(LogLLM, Warning, TEXT("HandlError: %s, code=%d"), *ErrorMessage, ErrorCode);
	OnLLMError.Broadcast(ActiveRequestId, ErrorMessage);
	
	ActiveRequestId.Invalidate();
	ActiveOwner = nullptr;
	
	TryStartNextRequest();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::TryStartNextRequest()
{
	if (ActiveRequestId.IsValid())
		return;
	
	if (PendingRequests.IsEmpty())
		return;
	
	if (!LlamaComponent)
		return;
	
	const FDialogueRequest Request = PendingRequests[0];
	PendingRequests.RemoveAt(0);
	StartRequestInternal(Request);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::StartRequestInternal(const FDialogueRequest& Request)
{
	ActiveBuffer.Empty();
	
	ActiveOwner = Request.Owner;
	ActiveRequestId = Request.RequestId;
	
	int32 LastUserMessage = INDEX_NONE;
	for (int32 I=Request.Messages.Num()-1; I >= 0; --I)
	{
		const FDialogueMessage& Message = Request.Messages[I];
		if (Message.Role == EChatTemplateRole::User)
		{
			LastUserMessage = I;
			break;
		}
	}
	
	UE_LOG(LogLLM, Log, TEXT("Send Prompt:"));
	for (int32 I=0; I < Request.Messages.Num(); ++I)
	{
		const FDialogueMessage& Message = Request.Messages[I];
		const bool bGenerateResponse = I == LastUserMessage;
		
		LlamaComponent->InsertTemplatedPrompt(Message.Content, Message.Role, false, bGenerateResponse); 
		UE_LOG(LogLLM, Log, TEXT("%s"), *Message.Content);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::SanitizeInPlace(FString& InOutText)
{
	StripPrefixOnce(InOutText, TEXT("<|start_header_id|>assistant<|end_header_id|>"));
	InOutText = InOutText.TrimStart();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UDialogueLLMService::StripPrefixOnce(FString& InOutText, const FString& Prefix)
{
	if (InOutText.StartsWith(Prefix))
	{
		InOutText.RightChopInline(Prefix.Len(), EAllowShrinking::No);
		return true;
	}
	return false;
}
