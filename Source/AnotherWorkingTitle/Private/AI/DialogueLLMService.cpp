// (c) 2025 MK

#include "AI/DialogueLLMService.h"

#include "LlamaComponent.h"

DEFINE_LOG_CATEGORY(LogLLM);

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::Deinitialize()
{
	if (LlamaComponent)
	{
		LlamaComponent->OnModelLoaded.Clear();
		LlamaComponent->OnError.Clear();
		LlamaComponent->OnResponseGenerated.Clear();
		LlamaComponent->OnEndOfStream.Clear();
	}
	
	LlamaComponent = nullptr;
	PendingRequests.Reset();
	bModelReady = false;
	Super::Deinitialize();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::RegisterLlamaComponent(ULlamaComponent* InLlamaComponent)
{
	if (!EnsureModelCopiedToSavedModels(ModelFileName))
		return;
	
	LlamaComponent = InLlamaComponent;
	if (LlamaComponent)
	{
		LlamaComponent->OnModelLoaded.AddUniqueDynamic(this, &ThisClass::HandleModelLoaded);
		LlamaComponent->OnError.AddUniqueDynamic(this, &ThisClass::HandleError);
		LlamaComponent->OnResponseGenerated.AddUniqueDynamic(this, &ThisClass::HandleResponseGenerated);
		LlamaComponent->OnEndOfStream.AddUniqueDynamic(this, &ThisClass::HandleEndOfStream);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UDialogueLLMService::IsReady() const
{
	return bModelReady && IsValid(LlamaComponent);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::SetListener(ILLMServiceListener* InListener)
{
	Clear();
	ActiveOwner = InListener;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FGuid UDialogueLLMService::EnqueueRequest(FDialogueRequest& Request)
{
	check(ActiveOwner);
	Request.RequestId = FGuid::NewGuid();
	PendingRequests.Add(Request);
	TryStartNextRequest();
	return Request.RequestId;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::Clear()
{
	if (LlamaComponent)
	{
		if (ActiveRequestId.IsValid())
		{
			bRequestStop = true;
			bResetHistoryAfterStop = true;
			LlamaComponent->StopGeneration();
		}
		else
		{
			bRequestStop = false;
			bResetHistoryAfterStop = false;
			LlamaComponent->ResetContextHistory();
		}
	}
	
	PendingRequests.Reset();
	
	ActiveRequestId.Invalidate();
	ActiveBuffer.Reset();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UDialogueLLMService::EnsureModelCopiedToSavedModels(const FString& InModelFileName) const
{
	if (InModelFileName.IsEmpty())
	{
		UE_LOG(LogLLM, Error, TEXT("EnsureModelCopiedToSavedModels: ModelFileName is empty."));
		return false;
	}

	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();

	const FString SavedModelsDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Models"));
	const FString DestPath      = FPaths::Combine(SavedModelsDir, InModelFileName);

	if (PF.FileExists(*DestPath))
	{
		return true;
	}

	const FString SrcPath = FPaths::Combine(FPaths::ProjectContentDir(), ContentModelSubDir, InModelFileName);

	if (!PF.FileExists(*SrcPath))
	{
		UE_LOG(LogLLM, Error, TEXT("LLM model not found at '%s'. Ensure it is staged as Non-UFS (Packaging -> Additional Non-Asset Directories to Copy)."), *SrcPath);
		return false;
	}

	if (!PF.CreateDirectoryTree(*SavedModelsDir))
	{
		UE_LOG(LogLLM, Error, TEXT("Failed to create directory '%s'."), *SavedModelsDir);
		return false;
	}

	if (!PF.CopyFile(*DestPath, *SrcPath))
	{
		UE_LOG(LogLLM, Error, TEXT("Failed to copy model from '%s' to '%s'."), *SrcPath, *DestPath);
		return false;
	}

	UE_LOG(LogLLM, Log, TEXT("Copied LLM model to '%s'."), *DestPath);
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandleModelLoaded(const FString& ModelName)
{
	bModelReady = true;
	UE_LOG(LogLLM, Log, TEXT("Model loaded: '%s'."), *ModelName);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandleResponseGenerated(const FString& FullText)
{
	UE_LOG(LogLLM, Log, TEXT("HandleResponseGenerated: %s"), *FullText);
	
	if (bRequestStop)
		return;

	if (!ActiveRequestId.IsValid())
		return;
	
	ActiveBuffer = FullText;
	SanitizeInPlace(ActiveBuffer);

	if (ActiveOwner)
	{
		ActiveOwner->OnResponseGenerated(ActiveRequestId, ActiveBuffer);
	}

	ActiveRequestId.Invalidate();
	
	TryStartNextRequest();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandleError(const FString& ErrorMessage, int32 ErrorCode)
{
	UE_LOG(LogLLM, Warning, TEXT("HandleError: %s, code=%d"), *ErrorMessage, ErrorCode);
	if (ActiveOwner)
	{
		ActiveOwner->OnError(ActiveRequestId, ErrorMessage);
	}
	ActiveBuffer.Reset();
	ActiveRequestId.Invalidate();
	
	bRequestStop = false;
	if (bResetHistoryAfterStop)
	{
		bResetHistoryAfterStop = false;
		if (LlamaComponent)
		{
			LlamaComponent->ResetContextHistory();
		}
	}
	
	TryStartNextRequest();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::HandleEndOfStream(bool bStopSequenceTriggered, float TokensPerSecond)
{
	const bool bWasRequested = bRequestStop;
	bRequestStop = false;
	if (bResetHistoryAfterStop)
	{
		bResetHistoryAfterStop = false;
		if (LlamaComponent)
		{
			LlamaComponent->ResetContextHistory();
		}
	}
	if (bWasRequested)
	{
		TryStartNextRequest();
	}
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
	
	if (!bModelReady)
		return;
	
	if (bRequestStop)
		return;
	
	const FDialogueRequest Request = PendingRequests[0];
	PendingRequests.RemoveAt(0);
	StartRequestInternal(Request);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UDialogueLLMService::StartRequestInternal(const FDialogueRequest& Request)
{
	ActiveBuffer.Empty();
	
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
