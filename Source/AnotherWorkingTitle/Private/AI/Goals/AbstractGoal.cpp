// (c) 2024 by Crenetic GmbH Studios

#include "AI/Goals/AbstractGoal.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString UAbstractGoal::GetTypeName() const
{
	const UClass* GoalClass = GetClass();
	check(GoalClass);
	const FName ClassName = GoalClass->GetFName();
	const FString ClassString = ClassName.ToString();
	return ClassString.RightChop(4);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
float UAbstractGoal::Evaluate(IAgent& Agent) const
{
	checkf(false, TEXT("Shouldn't be called, it's abstract"));
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UAbstractGoal::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	checkf(false, TEXT("Shouldn't be called, it's abstract"));
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractGoal::DeInit(IAgent& Agent, bool bIsSuccess) const
{
	checkf(false, TEXT("Shouldn't be called, it's abstract"));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString UAbstractGoal::Describe(IAgent& Agent, const FWorldState& WorldState) const
{
	return TEXT("You are doing nothing worth mentioning.");
}
