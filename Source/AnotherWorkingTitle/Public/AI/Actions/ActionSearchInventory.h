// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractAction.h"
#include "ActionSearchInventory.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UActionSearchInventory : public UAbstractAction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UActionSearchInventory();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void ApplyResults(IAgent& Agent, FWorldState& CurrentWorldState, const FWorldState& GoalWorldState) const override;
	virtual bool AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const override;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const override;
	virtual void Deactivate(IAgent& Agent, FAIState& AIState) const override;
	virtual EActionResult IsComplete(IAgent& Agent, FAIState& AIState) const override;
};
