// (c) 2024 by Crenetic GmbH Studios

#include "AI/Goals/GoalIdle.h"

#include "AI/AIHelper.h"
#include "AI/Planning/WorldState.h"
#include "AI/IAgent.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
float UGoalIdle::Evaluate(IAgent& Agent) const
{
	float Priority = FMath::Lerp(0.01f, 0.4f, Agent.GetNeedValue(ENeedType::Fatigue));
	if (Agent.HasMood(MOOD_3(EAgentMood::Stressed, EAgentMood::Afraid, EAgentMood::Angry)))
	{
		Priority += 0.5f;
	}
	return FAIHelper::Clamp01(Priority);
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
