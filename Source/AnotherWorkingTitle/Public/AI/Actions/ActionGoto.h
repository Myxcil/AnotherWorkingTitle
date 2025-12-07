#pragma once

#include "AbstractAction.h"
#include "ActionGoto.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class UActionGoto : public UAbstractDefaultAction
{
	GENERATED_BODY()

public:
	UActionGoto();

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual int32 GetPrecedence() const override { return 10; }
	virtual int32 GetCost(IAgent& Agent, const FWorldState& WorldState) const override;
	virtual float CalculateRuntimeCost(IAgent& Agent, const FWorldState& WorldState, FVector& PlannedLocation) const override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const override;
	virtual void Deactivate(IAgent& Agent) const override;
	virtual EActionResult IsComplete(IAgent& Agent) const override;
};
