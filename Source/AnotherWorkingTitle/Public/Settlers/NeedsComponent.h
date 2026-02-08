// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Needs.h"
#include "Components/ActorComponent.h"
#include "NeedsComponent.generated.h"

class ASettlerCharacter;
class ANeedModifierArea;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE(FNeedSeverityChanged);
DECLARE_DELEGATE(FDamagedReachedMaximum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNeedsMofifierChanged, ANeedModifierArea*, NeedsModifier, bool, bEnter);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API UNeedsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UNeedsComponent();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	void CheckNeedSeverity();

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void TickNeeds(const float DeltaGameHour);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	const FNeeds& GetNeeds() const { return Needs; }
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintPure)
	float GetNeedValue(const ENeedType NeedType) const;
	
	bool ChangeNeedValue(const ENeedType NeedType, const float Delta);

	UFUNCTION(BlueprintPure)
	ENeedSeverity CalculateSeverity(const ENeedType NeedType) const;
	
	bool IsAnyNeedInSeverityLevel(const ENeedSeverity NeedSeverity) const;
	
	void QueryState(FString& Output) const;
	
	FNeedSeverityChanged OnNeedSeverityChanged;
	FDamagedReachedMaximum OnDamagedReachedMaximum;
	UPROPERTY(BlueprintAssignable)
	FNeedsMofifierChanged OnNeedsModifierChanged;

private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category="Needs", meta=(AllowPrivateAccess=true))
	FNeeds Needs;

	UPROPERTY(EditAnywhere, Category="Needs|Rates", meta=(AllowPrivateAccess=true))
	float HungerRatePerHour = 0.083f;
	UPROPERTY(EditAnywhere, Category="Needs|Rates", meta=(AllowPrivateAccess=true))
	float ThirstRatePerHour = 0.12f;
	UPROPERTY(EditAnywhere, Category="Needs|Rates", meta=(AllowPrivateAccess=true))
	float ColdRatePerHour = 0.2f;
	UPROPERTY(EditAnywhere, Category="Needs|Rates", meta=(AllowPrivateAccess=true))
	float FatigueRatePerHour = 0.05f;

	UPROPERTY(EditAnywhere, Category="Needs|Rates", meta=(AllowPrivateAccess=true))
	float FatigueRateScaleSprint = 1.7f;
	
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalHunger = 0.1f;
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalThirst = 0.1f;
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalCold = 0.1f;
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalFatigue = 0.1f;

	UPROPERTY(EditAnywhere, Category="Needs|Thresholds", meta=(AllowPrivateAccess=true))
	float UncomfortableThreshold = 0.4f;
	UPROPERTY(EditAnywhere, Category="Needs|Thresholds", meta=(AllowPrivateAccess=true))
	float CriticalThreshold = 0.8f;
	
	
	UPROPERTY(EditAnywhere, Category="Needs|Cheats", meta=(AllowPrivateAccess=true))
	bool bInvincible = false;
	UPROPERTY(EditAnywhere, Category="Needs|Cheats", meta=(AllowPrivateAccess=true))
	bool bUndead = false;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void SetNeedValue(const ENeedType NeedType, const float InValue);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	TWeakObjectPtr<ASettlerCharacter> SettlerCharacterPtr;
	TStaticArray<ENeedSeverity, 6> PrevNeedSeverity;
};