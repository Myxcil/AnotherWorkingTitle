// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "AbstractAction.h"
#include "ActionSearchPath.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class UActionSearchPath : public UAbstractDefaultAction
{
	GENERATED_BODY()

public:
	UActionSearchPath();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const override;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const override;
	virtual void Deactivate(IAgent& Agent) const override;
	virtual EActionResult IsComplete(IAgent& Agent) const override;
};
