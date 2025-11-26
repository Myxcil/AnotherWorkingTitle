// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractGoalCollectResource.h"
#include "GoalCollectWood.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UGoalCollectWood : public UAbstractGoalCollectResource
{
	GENERATED_BODY()
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual const UResourceDefinition* GetResource(IAgent& Agent) const override;	
};
