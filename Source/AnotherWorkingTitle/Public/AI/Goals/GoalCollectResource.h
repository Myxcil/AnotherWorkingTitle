// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractGoal.h"
#include "GoalCollectResource.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class UGoalCollectResource : public UAbstractGoal
{
	GENERATED_BODY()
	
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual float Evaluate(IAgent& Agent) const override;
	virtual bool Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const override;
	virtual void DeInit(IAgent& Agent, bool bIsSuccess) const override;
};
