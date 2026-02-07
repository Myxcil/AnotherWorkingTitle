#pragma once

#include "CoreMinimal.h"
#include "AI/AIConstants.h"
#include "AbstractGoal.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
class IAgent;
class FWorldState;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class UAbstractGoal : public UObject
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ANOTHERWORKINGTITLE_API FString GetTypeName() const;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool CanSkipApproval() const { return true; }
	virtual uint32 GetMatchFlags() const { return 0xffffffff; }

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual float Evaluate(IAgent& Agent) const;
	virtual bool Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const;
	virtual void DeInit(IAgent& Agent, bool bIsSuccess) const;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual FString Describe(IAgent& Agent, const FWorldState& WorldState) const;
};
