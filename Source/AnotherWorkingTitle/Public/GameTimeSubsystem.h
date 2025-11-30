// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameTimeSubsystem.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameDayChanged);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable, Abstract)
class ANOTHERWORKINGTITLE_API UGameTimeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void OnWorldEndPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintPure)
	float GetGameDeltaHour() const { return LastGameDeltaHour; } 
	UFUNCTION(BlueprintPure)
	float GetNormalizedGameTime() const { return FMath::Clamp(AccumulatedDayTimeHours / 24.0f, 0.0f, 1.0f); };
	UFUNCTION(BlueprintCallable)
	FText GetGameTimeFormatted() const;
	
	float GetGameHourToRealSeconds(const float InGameHour) const;
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GameTime")
	float GameHoursPerRealSecond = 1.0f / 60.0f;
	
	UPROPERTY(BlueprintAssignable)
	FGameDayChanged OnGameDayChanged;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	float LastGameDeltaHour = 0;
	float AccumulatedDayTimeHours = 0;
	int32 GameDay = 0;
};
