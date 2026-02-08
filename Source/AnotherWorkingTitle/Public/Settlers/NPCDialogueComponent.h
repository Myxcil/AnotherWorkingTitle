// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/DialogueLLMService.h"
#include "NPCDialogueComponent.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCDialogueToken, FGuid, RequestId, const FString&, TokenOrChunk);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCDialogueResponse, FGuid, RequestId, const FString&, FullText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCDialogueError, FGuid, RequestId, const FString&, ErrorText);

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API UNPCDialogueComponent : public UActorComponent, public ILLMServiceListener
{
	GENERATED_BODY()

public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UNPCDialogueComponent();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// How many turns to keep in memory (per NPC). Keep small initially.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dialogue")
	int32 MaxHistoryMessages = 12;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void OnBeginDialog();
	UFUNCTION(BlueprintCallable)
	void OnEndDialog();
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Build + send a request. Returns RequestId.
	UFUNCTION(BlueprintCallable)
	void SendPlayerLine(const FString& PlayerText);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Cancel active/pending requests originating from this component.
	UFUNCTION(BlueprintCallable)
	void CancelAllRequests();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// ILLMServiceListener
	virtual void OnTokenReceived(const FGuid& RequestId, const FString& Token) override;
	virtual void OnResponseGenerated(const FGuid& RequestId, const FString& FullText) override;
	virtual void OnError(const FGuid& RequestId, const FString& ErrorText) override;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// UI/consumer binds to these.
	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueToken OnToken;

	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueResponse OnResponse;

protected:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Local convo memory (per NPC).
	UPROPERTY()
	TArray<FDialogueMessage> History;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Keep track of outstanding requests for cancel/ignore.
	TArray<FGuid> OwnedRequestIds;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Keep track of first request, which will contain additional info
	bool bFirstRequest = true;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UDialogueLLMService* GetService() const;

	void PruneHistoryIfNeeded();
	TArray<FDialogueMessage> BuildMessagesForRequest(const FString& PlayerText) const;
};
