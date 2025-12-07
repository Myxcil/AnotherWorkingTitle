#pragma once

#include "AbstractGoal.h"
#include "GoalIdle.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class UGoalIdle : public UAbstractGoal
{
	GENERATED_BODY()

public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual float Evaluate(IAgent& Agent) const override;
	virtual bool Init(IAgent& Agent, FWorldState& WorldState, const bool bIsPlanning) const override;
	virtual void DeInit(IAgent& Agent, bool bIsSuccess) const override;
};
