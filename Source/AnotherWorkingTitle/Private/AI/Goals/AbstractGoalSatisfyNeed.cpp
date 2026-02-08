// (c) 2025 MK


#include "AI/Goals/AbstractGoalSatisfyNeed.h"

#include "AI/AIBlackboard.h"
#include "AI/IAgent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float UAbstractGoalSatisfyNeed::Evaluate(IAgent& Agent) const
{
	const ENeedType NeedType = GetNeedType(); 
	const float NeedRawValue = Agent.GetNeedValue(NeedType);
	
	return NeedRawValue;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UAbstractGoalSatisfyNeed::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	const ENeedType NeedType = GetNeedType(); 
	WorldState.Set(EWorldPropertyKey::Need, NeedType);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractGoalSatisfyNeed::DeInit(IAgent& Agent, bool bIsSuccess) const
{
	Agent.GetBlackboard().Clear(EBlackboardMask::ResourceNode);
	Agent.GetBlackboard().Clear(EBlackboardMask::Stockpile);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FString UAbstractGoalSatisfyNeed::Describe(IAgent& Agent, const FWorldState& WorldState) const
{
	const ENeedType NeedType = GetNeedType();
	switch (NeedType)
	{
	case ENeedType::Unknown:
		break;
	case ENeedType::Hunger:
		return TEXT("looking for food");
	case ENeedType::Thirst:
		return TEXT("looking for drink");
	case ENeedType::Cold:
		return TEXT("looking for heat");
	case ENeedType::Fatigue:
		return TEXT("looking for rest");
	case ENeedType::Damage:
		return TEXT("looking for healing");
	}
	return Super::Describe(Agent, WorldState);
}
