#pragma once 

#include "AISettings.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(config=GOAP, defaultconfig, DisplayName="GOAP AI Settings", dontCollapseCategories)
class ANOTHERWORKINGTITLE_API UAISettings : public UObject
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Config, Category = "General")
	bool bUseGoalDelay = true;
	UPROPERTY(EditDefaultsOnly, Config, Category = "General")
	float GoalExecuteMinDelay = 0.1f;
	UPROPERTY(EditDefaultsOnly, Config, Category = "General")
	float GoalExecuteMaxDelay = 0.5f;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Config, Category = "Movement")
	float NearRadius = 100.0f;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Config, Category = "Waiting")
	int32 MinIdleWaitDuration = 20.0f;
	UPROPERTY(EditDefaultsOnly, Config, Category = "Waiting")
	int32 MaxIdleWaitDuration = 60.0f;
};