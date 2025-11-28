// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Needs.h"
#include "Components/ActorComponent.h"
#include "NeedsComponent.generated.h"

class ANeedsModifier;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DELEGATE(FDamagedReachedMaximum)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNeedsMofifierChanged, ANeedsModifier*, NeedsModifier, bool, bEnter);

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

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	const FNeeds& GetNeeds() const { return Needs; }
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintPure)
	float GetNeedValue(const ENeedType NeedType) const;
	
	bool ChangeNeedValue(const ENeedType NeedType, const float Delta);

	UFUNCTION(BlueprintPure)
	bool IsNeedUncomfortable(const ENeedType NeedType) const { return GetNeedValue(NeedType) >= UncomfortableThreshold; }
	UFUNCTION(BlueprintPure)
	bool IsNeedCritical(const ENeedType NeedType) const { return GetNeedValue(NeedType) >= CriticalThreshold; }
	
	bool IsAnyNeedUncomfortable() const;
	bool IsAnyNeedCritical() const;
	
	FDamagedReachedMaximum OnDamagedReachedMaximum;
	UPROPERTY(BlueprintAssignable)
	FNeedsMofifierChanged OnNeedsModifierChanged;

private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category="Needs", meta=(AllowPrivateAccess=true))
	FNeeds Needs;

	UPROPERTY(EditAnywhere, Category="Needs|Config", meta=(AllowPrivateAccess=true))
	float HungerRate = 0.001f;
	UPROPERTY(EditAnywhere, Category="Needs|Config", meta=(AllowPrivateAccess=true))
	float ThirstRate = 0.002f;
	UPROPERTY(EditAnywhere, Category="Needs|Config", meta=(AllowPrivateAccess=true))
	float ColdRate = 0.003f;
	UPROPERTY(EditAnywhere, Category="Needs|Config", meta=(AllowPrivateAccess=true))
	float FatigueRate = 0.0015f;
	
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalHunger = 0.001f;
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalThirst = 0.001f;
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalCold = 0.001f;
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalFatigue = 0.001f;

	UPROPERTY(EditAnywhere, Category="Needs|Thresholds", meta=(AllowPrivateAccess=true))
	float UncomfortableThreshold = 0.4f;
	UPROPERTY(EditAnywhere, Category="Needs|Thresholds", meta=(AllowPrivateAccess=true))
	float CriticalThreshold = 0.8f;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void SetNeedValue(const ENeedType NeedType, const float InValue);
};