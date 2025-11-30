// (c) 2024 by Crenetic GmbH Studios

#include "AI/Goals/GoalIdle.h"
#include "AI/Planning/WorldState.h"
#include "AI/IAgent.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
float UGoalIdle::Evaluate(IAgent& Agent) const
{
	return 0.0001f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGoalIdle::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	WorldState.Set(EWorldPropertyKey::Wait, true);
	if (!bIsPlanning)
	{
		Agent.SetSprinting(false);
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UGoalIdle::DeInit(IAgent& Agent, bool bIsSuccess) const
{
	Agent.SetSprinting(true);
}
