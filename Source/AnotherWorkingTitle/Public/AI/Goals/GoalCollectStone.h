// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractGoalCollectResource.h"
#include "GoalCollectStone.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UGoalCollectStone : public UAbstractGoalCollectResource
{
	GENERATED_BODY()
	
public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual FString Describe(IAgent& Agent, const FWorldState& WorldState) const override;	
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual const UResourceDefinition* GetResource(IAgent& Agent) const override;	
};
