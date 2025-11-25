#pragma once

#include "AbstractAction.h"
#include "ActionWait.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class UActionWait : public UAbstractDefaultAction
{
	GENERATED_BODY()
	
public:
	UActionWait();
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual int32 GetPrecedence() const override { return 10; }

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const override;
	virtual void Deactivate(IAgent& Agent, FAIState& AIState) const override;
	virtual EActionResult IsComplete(IAgent& Agent, FAIState& AIState) const override;
};
