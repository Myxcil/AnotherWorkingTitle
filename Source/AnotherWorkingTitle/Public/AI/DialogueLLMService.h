// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "LlamaDataTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Misc/Guid.h"
#include "DialogueLLMService.generated.h"

class ULlamaComponent;

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
struct FDialogueGenerationParams
{
	GENERATED_BODY()

	// Keep this small for "as fast as possible" default; you can override per request.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxNewTokens = 128;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bStream = true;

	// Optional: temperature/top_p/etc. Add later if needed.
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FDialogueRequest
{
	GENERATED_BODY()

	// Owning object for cancellation/routing (typically a UNPCDialogueComponent or UI controller)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UObject> Owner = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString DebugName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FDialogueMessage> Messages;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDialogueGenerationParams Gen;

	// Filled by service on enqueue.
	FGuid RequestId;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLLMReady, bool, bReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLLMToken, FGuid, RequestId, const FString&, TokenOrChunk);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLLMResponse, FGuid, RequestId, const FString&, FullText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLLMError, FGuid, RequestId, const FString&, ErrorText);

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

	UFUNCTION(BlueprintCallable)
	bool IsReady() const;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	FGuid EnqueueRequest(UPARAM(ref) FDialogueRequest& Request);

	UFUNCTION(BlueprintCallable)
	void CancelRequest(const FGuid& RequestId);

	UFUNCTION(BlueprintCallable)
	void CancelAllForOwner(UObject* Owner);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintAssignable)
	FOnLLMReady OnLLMReady;

	UPROPERTY(BlueprintAssignable)
	FOnLLMToken OnLLMToken;

	UPROPERTY(BlueprintAssignable)
	FOnLLMResponse OnLLMResponse;

	UPROPERTY(BlueprintAssignable)
	FOnLLMError OnLLMError;

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY()
	TObjectPtr<ULlamaComponent> LlamaComponent = nullptr;

	bool bModelReady = false;

	TArray<FDialogueRequest> PendingRequests;

	FGuid ActiveRequestId;
	TWeakObjectPtr<UObject> ActiveOwner;
	FString ActiveBuffer;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION()
	void HandleModelLoaded(const FString& ModelName);

	UFUNCTION()
	void HandleNewToken(const FString& Token);

	UFUNCTION()
	void HandlePartialGenerated(const FString& Chunk);

	UFUNCTION()
	void HandleResponseGenerated(const FString& FullText);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void TryStartNextRequest();
	void StartRequestInternal(const FDialogueRequest& Request);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	static void SanitizeInPlace(FString& InOutText);
	static bool StripPrefixOnce(FString& InOutText, const FString& Prefix);
};
