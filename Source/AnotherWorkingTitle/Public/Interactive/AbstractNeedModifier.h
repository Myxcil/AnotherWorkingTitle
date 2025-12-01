// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Construction/BaseBuilding.h"
#include "Settlers/Needs.h"
#include "AbstractNeedModifier.generated.h"

class UGameTimeSubsystem;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class ANOTHERWORKINGTITLE_API AAbstractNeedModifier : public ABaseBuilding
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	AAbstractNeedModifier();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	
public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ENeedType GetAffectedType() const { return AffectedType; }
	float GetNeedDelta() const { return NeedsValueDelta; }

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	float GetDeltaTimeHour() const;
	float GetGameHourToRealSeconds(const float GameHour) const;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	ENeedType AffectedType = ENeedType::Fatigue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	float NeedsValueDelta = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	TObjectPtr<UTexture2D> EffectIcon = nullptr;

private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	TWeakObjectPtr<const UGameTimeSubsystem> GameTimeSubsystemPtr;
};
