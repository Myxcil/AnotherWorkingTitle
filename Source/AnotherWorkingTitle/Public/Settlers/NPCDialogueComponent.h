// (c) 2025 MK

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
	// ILLMServiceListener
	virtual void OnResponseGenerated(const FGuid& RequestId, const FString& FullText) override;
	virtual void OnError(const FGuid& RequestId, const FString& ErrorText) override;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// UI/consumer binds to these.
	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueResponse OnResponse;

protected:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UDialogueLLMService* GetService() const;
	void PruneHistoryIfNeeded();
	TArray<FDialogueMessage> BuildMessagesForRequest(const FString& PlayerText) const;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	TArray<FDialogueMessage> History;
	TArray<FGuid> OwnedRequestIds;
	bool bFirstRequest = true;
	bool bInDialog = false;
};
