// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractAction.h"
#include "ActionSearchResourceNode.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UActionSearchResourceNode : public UAbstractAction
{
	GENERATED_BODY()
	
public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UActionSearchResourceNode();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void ApplyPreconditions(IAgent& Agent, FWorldState& GoalState) const override;
	virtual bool ArePreconditionsSatisfied(const FWorldState& CurrentWorldState, const FWorldState& GoalState, EWorldPropertyKey& FailedKey) const override;
	
	virtual bool AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const override;
	virtual void Deactivate(IAgent& Agent, FAIState& AIState) const override;
	virtual EActionResult IsComplete(IAgent& Agent, FAIState& AIState) const override;
};
