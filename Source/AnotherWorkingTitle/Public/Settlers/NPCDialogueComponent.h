// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/DialogueLLMService.h"
#include "NPCDialogueComponent.generated.h"

class UGOAPAgentComponent;
class UNeedsComponent;
class USocialComponent;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class UDialogueGlobalPromptAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(MultiLine=true))
	FString Rules;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(MultiLine=true))
	FString Lore;
};

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
	UFUNCTION(BlueprintCallable)
	void OnBeginDialog();
	UFUNCTION(BlueprintCallable)
	void OnEndDialog();
	UFUNCTION(BlueprintCallable)
	void SendPlayerLine(const FString& PlayerText);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// ILLMServiceListener
	virtual void OnResponseGenerated(const FGuid& RequestId, const FString& FullText) override;
	virtual void OnError(const FGuid& RequestId, const FString& ErrorText) override;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueResponse OnResponse;

protected:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDialogueGlobalPromptAsset> GlobalPromptAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine=true))
	FString Persona;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dialogue")
	int32 MaxHistoryMessages = 12;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	struct FWorldSnapshot
	{
		FString	Mood;
		FString	Relationship;
		FString Need;
		FString Goal;
	};
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UDialogueLLMService* GetService() const;
	void PruneHistoryIfNeeded();
	TArray<FDialogueMessage> BuildMessagesForRequest(const FString& PlayerText);
	
	void TakeWorldSnapshot(FWorldSnapshot& Snapshot) const;
	FString GenerateWorldSnapshotMessage(const FWorldSnapshot& Snapshot) const;
		
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY()
	TObjectPtr<USocialComponent> Social = nullptr;
	UPROPERTY()
	TObjectPtr<UNeedsComponent> Needs = nullptr;
	UPROPERTY()
	TObjectPtr<UGOAPAgentComponent> Agent = nullptr;
	UPROPERTY()
	TObjectPtr<USocialComponent> PlayerSocial = nullptr;
	
	TArray<FDialogueMessage> History;
	TArray<FGuid> OwnedRequestIds;
	bool bFirstRequest = true;
	bool bInDialog = false;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorldSnapshot CachedSnapshot;
};
