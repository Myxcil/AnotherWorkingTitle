// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractGoal.h"
#include "AbstractGoalCollectResource.generated.h"

class UResourceDefinition;
//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class UAbstractGoalCollectResource : public UAbstractGoal
{
	GENERATED_BODY()
	
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual float Evaluate(IAgent& Agent) const override;
	virtual bool Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const override;
	virtual void DeInit(IAgent& Agent, bool bIsSuccess) const override;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual FString Describe(IAgent& Agent, const FWorldState& WorldState) const override;
	
protected:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual const UResourceDefinition* GetResource(IAgent& Agent) const { return nullptr; }
};
