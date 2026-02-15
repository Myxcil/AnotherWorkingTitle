// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/DialogueLLMService.h"
#include "NPCDialogueComponent.generated.h"

class UGOAPAgentComponent;
class UNeedsComponent;
class USocialComponent;
class UTtsPiperStreamComponent;

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
	virtual void OnTokenGenerated(const FGuid& RequestId, const FString& TokenOrChunk) override;
	virtual void OnResponseGenerated(const FGuid& RequestId, const FString& FullText) override;
	virtual void OnError(const FGuid& RequestId, const FString& ErrorText) override;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueToken OnNPCDialogueToken;
	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueResponse OnResponse;
	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueError OnDialogueError;

protected:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDialogueGlobalPromptAsset> GlobalPromptAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine=true))
	FString Persona;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dialogue")
	int32 MaxHistoryMessages = 12;

	// Optional: enable/disable speaking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue|TTS")
	bool bSpeakNPCResponse = true;

	// Chunking parameters (low latency vs naturalness)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue|TTS")
	int32 TTS_MinChars = 40;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue|TTS")
	int32 TTS_TargetChars = 120;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue|TTS")
	int32 TTS_MaxChars = 220;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue|TTS")
	float TTS_IdleFlushSeconds = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue|TTS")
	bool bTTS_SplitOnComma = true;

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
		
	UFUNCTION()
	void HandleTtsFinished(const FPiperTtsResult& Result);

	void FeedAssistantDelta(const FGuid& RequestId, const FString& Delta, bool bFinal);
	void EnqueueSegmentsFromBuffer(bool bForceFlush);
	int32 FindSplitIndex(bool bForceFlush) const;
	void TrySpeakNext();
	void FlushIdle();

	// helpers
	static bool IsSentenceEnd(TCHAR C);
	static bool IsSoftBreak(TCHAR C);

	// --- runtime ---
	UPROPERTY()
	TObjectPtr<UTtsPiperStreamComponent> Tts = nullptr;

	FGuid ActiveSpeechRequestId;
	bool bReceivedAnyToken = false;
	bool bTtsBusy = false;

	FString AssistantBuffer;
	TArray<FString> SegmentQueue;

	FTimerHandle IdleFlushTimer;
	
	UPROPERTY(Transient)
	TObjectPtr<USocialComponent> Social = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UNeedsComponent> Needs = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UGOAPAgentComponent> Agent = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<USocialComponent> PlayerSocial = nullptr;
	
	TArray<FDialogueMessage> History;
	TArray<FGuid> OwnedRequestIds;
	bool bFirstRequest = true;
	bool bInDialog = false;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorldSnapshot CachedSnapshot;
};
