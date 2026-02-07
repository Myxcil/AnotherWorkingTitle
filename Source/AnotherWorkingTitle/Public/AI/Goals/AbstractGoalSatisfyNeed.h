// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractGoal.h"
#include "Settlers/Needs.h"
#include "AbstractGoalSatisfyNeed.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class ANOTHERWORKINGTITLE_API UAbstractGoalSatisfyNeed : public UAbstractGoal
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual float Evaluate(IAgent& Agent) const override;
	virtual bool Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const override;
	virtual void DeInit(IAgent& Agent, bool bIsSuccess) const override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual FString Describe(IAgent& Agent, const FWorldState& WorldState) const override;
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual ENeedType GetNeedType() const { return static_cast<ENeedType>(255); }
};
