#pragma once

#include "CoreMinimal.h"
#include "AI/Planning/WorldState.h"
#include "AbstractAction.generated.h"

struct FAIState;
class IAgent;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class EActionResult : uint8 
{
	Incomplete,
	Complete,
	Failed,	
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class UAbstractAction : public UObject
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	FString ANOTHERWORKINGTITLE_API GetTypeName() const;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool HasResult(const EWorldPropertyKey Key) const { return Results.IsValid(Key); }

	virtual int32 GetPrecedence() const { return 0; }
	virtual int32 GetCost(IAgent& Agent, const FWorldState& WorldState) const { return 1; }
	virtual float CalculateRuntimeCost(IAgent& Agent, const FWorldState& WorldState, FVector& PlannedLocation) const { return 1.0f; }

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool CanSolveWorldState(const FWorldState& CurrentWorldState, const FWorldState& GoalWorldState) const;

	virtual void ApplyPreconditions(IAgent& Agent, FWorldState& GoalState) const;
	virtual void ApplyResults(IAgent& Agent, FWorldState& CurrentWorldState, const FWorldState& GoalWorldState) const;
	virtual void SolveWorldState(FWorldState& CurrentWorldState, const FWorldState& GoalWorldState) const;

	virtual bool ArePreconditionsSatisfied(const FWorldState& CurrentWorldState, const FWorldState& GoalState, EWorldPropertyKey& FailedKey) const;
	virtual bool AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const { return true; }

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const;
	virtual void Deactivate(IAgent& Agent) const;
	virtual EActionResult IsComplete(IAgent& Agent) const;

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	FWorldState Preconditions;
	FWorldState Results;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class UAbstractDefaultAction : public UAbstractAction
{
	GENERATED_BODY()
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class UAbstractSpecialAction : public UAbstractAction
{
	GENERATED_BODY()
};

