// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractGoalSatisfyNeed.h"
#include "GoalSatisfyCold.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UGoalSatisfyCold : public UAbstractGoalSatisfyNeed
{
	GENERATED_BODY()

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual ENeedType GetNeedType() const override { return ENeedType::Cold; }
};
