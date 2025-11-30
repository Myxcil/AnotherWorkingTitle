// (c) 2025 MK


#include "AI/Goals/AbstractGoalSatisfyNeed.h"

#include "AI/AIBlackboard.h"
#include "AI/IAgent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float UAbstractGoalSatisfyNeed::Evaluate(IAgent& Agent) const
{
	const ENeedType NeedType = GetNeedType(); 
	if (Agent.GetNeedSeverity(NeedType) == ENeedSeverity::Normal)
		return 0;
	
	return Agent.GetNeedValue(NeedType);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UAbstractGoalSatisfyNeed::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	const ENeedType NeedType = GetNeedType(); 
	WorldState.Set(EWorldPropertyKey::UseSlot, NeedType);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractGoalSatisfyNeed::DeInit(IAgent& Agent, bool bIsSuccess) const
{
	Agent.GetBlackboard().Clear(EBlackboardMask::SlotIndex);
	Agent.GetBlackboard().Clear(EBlackboardMask::ResourceNode);
	Agent.GetBlackboard().Clear(EBlackboardMask::Stockpile);
}
