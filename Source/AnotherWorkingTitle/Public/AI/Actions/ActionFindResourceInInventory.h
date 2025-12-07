// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractAction.h"
#include "ActionFindResourceInInventory.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UActionFindResourceInInventory : public UAbstractAction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UActionFindResourceInInventory();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const override;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const override;
	virtual void Deactivate(IAgent& Agent) const override;
	virtual EActionResult IsComplete(IAgent& Agent) const override;
};
