// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/DialogueLLMService.h"
#include "NPCDialogueComponent.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FDialogueContextDescriptor
{
	GENERATED_BODY()

	// Keep as labels (not floats) for speed and stability.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString MoodLabel;           // e.g. "calm", "angry", "fearful", "disgusted"

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString RelationshipLabel;   // e.g. "trusted", "wary", "suspicious", "hostile"

	// Optional concise facts (already curated).
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FString> Facts;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCDialogueToken, FGuid, RequestId, const FString&, TokenOrChunk);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCDialogueResponse, FGuid, RequestId, const FString&, FullText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCDialogueError, FGuid, RequestId, const FString&, ErrorText);

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API UNPCDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	UNPCDialogueComponent();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Persona as short text block (2-6 sentences), English.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dialogue")
	FString PersonaText;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// How many turns to keep in memory (per NPC). Keep small initially.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dialogue")
	int32 MaxHistoryMessages = 12;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// --- API used by UI / interaction ---
	UFUNCTION(BlueprintCallable)
	void ResetConversation();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Build + send a request. Returns RequestId.
	UFUNCTION(BlueprintCallable)
	FGuid SendPlayerLine(const FString& PlayerText, const FDialogueContextDescriptor& Context, const FDialogueGenerationParams& GenParams);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Cancel active/pending requests originating from this component.
	UFUNCTION(BlueprintCallable)
	void CancelAllRequests();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// UI/consumer binds to these.
	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueToken OnToken;

	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueResponse OnResponse;

	UPROPERTY(BlueprintAssignable)
	FOnNPCDialogueError OnError;

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
	UDialogueLLMService* GetService() const;

	void PruneHistoryIfNeeded();
	TArray<FDialogueMessage> BuildMessagesForRequest(const FString& PlayerText, const FDialogueContextDescriptor& Context) const;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// Handlers wired to service events; filter by request id owner.
	UFUNCTION()
	void HandleServiceToken(FGuid RequestId, const FString& TokenOrChunk);

	UFUNCTION()
	void HandleServiceResponse(FGuid RequestId, const FString& FullText);

	UFUNCTION()
	void HandleServiceError(FGuid RequestId, const FString& ErrorText);
};
