#pragma once

#include "CoreMinimal.h"
#include "AI/AIConstants.h"
#include "AbstractGoal.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class IAgent;
class FWorldState;

namespace GoalPriorities
{
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	const TRange<float> PriorityScaleCollect(0.1f, 0.7f);
	const TRange<float> PriorityScaleDeposit( 0.1f, 0.6f);
	const TRange<float> PriorityScaleConstruct( 0.15f, 0.75);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class UAbstractGoal : public UObject
{
	GENERATED_BODY()

public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	ANOTHERWORKINGTITLE_API FString GetTypeName() const;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool CanSkipApproval() const { return true; }
	virtual uint32 GetMatchFlags() const { return 0xffffffff; }

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual float Evaluate(IAgent& Agent) const;
	virtual bool Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const;
	virtual void DeInit(IAgent& Agent, bool bIsSuccess) const;
};
