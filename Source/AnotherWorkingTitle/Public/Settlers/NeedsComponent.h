// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Needs.h"
#include "Components/ActorComponent.h"
#include "NeedsComponent.generated.h"


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
	
	void ChangeNeedValue(const ENeedType NeedType, const float Delta);

	UFUNCTION(BlueprintPure)
	bool IsNeedUncomfortable(const ENeedType NeedType) const { return GetNeedValue(NeedType) >= UncomfortableThreshold; }
	UFUNCTION(BlueprintPure)
	bool IsNeedCritical(const ENeedType NeedType) const { return GetNeedValue(NeedType) >= CriticalThreshold; } 

private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category="Needs", meta=(AllowPrivateAccess=true))
	FNeeds Needs;

	UPROPERTY(EditAnywhere, Category="Needs|Config", meta=(AllowPrivateAccess=true))
	float HungerRatePerHour = 0.1f;
	UPROPERTY(EditAnywhere, Category="Needs|Config", meta=(AllowPrivateAccess=true))
	float ThirstRatePerHour = 0.2f;
	UPROPERTY(EditAnywhere, Category="Needs|Config", meta=(AllowPrivateAccess=true))
	float FatigueRatePerHour = 0.15f;
	
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalHunger = 0.1f;
	UPROPERTY(EditAnywhere, Category="Needs|Damage", meta=(AllowPrivateAccess=true))
	float DamageForCriticalThirst = 0.1f;

	UPROPERTY(EditAnywhere, Category="Needs|Thresholds", meta=(AllowPrivateAccess=true))
	float UncomfortableThreshold = 0.4f;
	UPROPERTY(EditAnywhere, Category="Needs|Thresholds", meta=(AllowPrivateAccess=true))
	float CriticalThreshold = 0.8f;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void SetNeedValue(const ENeedType NeedType, const float InValue);
};