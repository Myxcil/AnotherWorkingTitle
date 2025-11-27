// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractAction.h"
#include "ActionConstruct.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UActionConstruct : public UAbstractAction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UActionConstruct();
	
	virtual bool AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const override;
	virtual void Deactivate(IAgent& Agent, FAIState& AIState) const override;
	virtual EActionResult IsComplete(IAgent& Agent, FAIState& AIState) const override;
};
