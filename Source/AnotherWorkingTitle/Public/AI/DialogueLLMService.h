// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "LlamaDataTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Misc/Guid.h"
#include "DialogueLLMService.generated.h"

class ULlamaComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogLLM, Log, All);

//--------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FDialogueMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EChatTemplateRole Role;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Content;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FDialogueRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FDialogueMessage> Messages;

	// Filled by service on enqueue.
	FGuid RequestId;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class ILLMServiceListener
{
public:
	virtual ~ILLMServiceListener() = default;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void OnResponseGenerated(const FGuid& RequestId, const FString& FullText) = 0;
	virtual void OnError(const FGuid& RequestId, const FString& ErrorText) = 0;	
};


//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UDialogueLLMService : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void RegisterLlamaComponent(ULlamaComponent* InLlamaComponent);
	bool IsReady() const;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void SetListener(ILLMServiceListener* InListener);
	FGuid EnqueueRequest(UPARAM(ref) FDialogueRequest& Request);

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY()
	TObjectPtr<ULlamaComponent> LlamaComponent = nullptr;

	bool bModelReady = false;

	ILLMServiceListener* ActiveOwner = nullptr;
	TArray<FDialogueRequest> PendingRequests;
	FGuid ActiveRequestId;
	FString ActiveBuffer;
	bool bRequestStop = false;
	bool bResetHistoryAfterStop = false;
	bool bRetryAfterStop = false;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FString ModelFileName = TEXT("llama-3.2-3b-instruct-q4_k_m.gguf");
	FString ContentModelSubDir = TEXT("LLMModels");

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	bool EnsureModelCopiedToSavedModels(const FString& InModelFileName) const;
	void Clear();
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION()
	void HandleModelLoaded(const FString& ModelName);

	UFUNCTION()
	void HandleResponseGenerated(const FString& FullText);

	UFUNCTION()
	void HandleError(const FString& ErrorMessage, int32 ErrorCode);
	
	UFUNCTION()
	void HandleEndOfStream(bool bStopSequenceTriggered, float TokensPerSecond);
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void TryStartNextRequest();
	void StartRequestInternal(const FDialogueRequest& Request);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	static void SanitizeInPlace(FString& InOutText);
	static bool StripPrefixOnce(FString& InOutText, const FString& Prefix);
};
