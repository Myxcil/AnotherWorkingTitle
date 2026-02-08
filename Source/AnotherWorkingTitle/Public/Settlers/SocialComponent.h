// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Emotions.h"
#include "Components/ActorComponent.h"
#include "SocialComponent.generated.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmotionalStateChanged);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API USocialComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	USocialComponent();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void Connect(USocialComponent* SocialComponent);
	void Disconnect(USocialComponent* SocialComponent);

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void TickSocial(const float DeltaGameHour);
	
	const FEmotionSummary& GetEmotionSummary() const { return CachedSummary; }
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintPure)
	FRelationshipState& GetRelationshipMutable(const USocialComponent* Other) { return RelationShips[Other]; }
	UFUNCTION(BlueprintPure)
	const FRelationshipState& GetRelationship(const USocialComponent* Other) const { return RelationShips[Other]; }

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintAssignable)
	FEmotionalStateChanged OnEmotionalStateChanged;
	UFUNCTION(BlueprintCallable)
	FText GetEmotionalState() const; 

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void QueryEmotionalState(FString& Output) const;
	bool QueryRelationship(FString& Output, const USocialComponent* Other) const;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
#if WITH_EDITOR
	UFUNCTION(CallInEditor)
	void RandomizeCurrentEmotion();
#endif
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void ApplyEmotion(const EPrimaryEmotionAxis EmotionAxis, const float Delta);
	void UpdateEmotionRegulation(const float DeltaGameHour);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UEmotionalArchetype> EmotionalArchetype = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FEmotionalState BaselineEmotion;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FEmotionalState CurrentEmotion;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FEmotionTemperament Temperament;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FEmotionalThresholds Thresholds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float GlobalIntensity = 1.0f;
	
	TMap<TWeakObjectPtr<USocialComponent>, FRelationshipState> RelationShips;
	
	bool bIsDirty = false;
	FEmotionSummary CachedSummary;
};
