// (c) 2025 MK


#include "AI/Goals/AbstractGoalSatisfyNeed.h"

#include "AI/IAgent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float UAbstractGoalSatisfyNeed::Evaluate(IAgent& Agent) const
{
	const ENeedType NeedType = GetNeedType(); 
	return Agent.GetNeedValue(NeedType);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UAbstractGoalSatisfyNeed::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	const ENeedType NeedType = GetNeedType(); 
	WorldState.Set(EWorldPropertyKey::SatisfyNeed, NeedType);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractGoalSatisfyNeed::DeInit(IAgent& Agent, bool bIsSuccess) const
{
}
