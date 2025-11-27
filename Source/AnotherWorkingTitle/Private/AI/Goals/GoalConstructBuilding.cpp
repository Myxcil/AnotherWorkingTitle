// (c) 2025 MK


#include "AI/Goals/GoalConstructBuilding.h"

#include "AI/AIGoalMapping.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "AI/Planning/WorldState.h"
#include "Construction/BuildingSite.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float UGoalConstructBuilding::Evaluate(IAgent& Agent) const
{
	if (Agent.IsInCriticalState())
		return 0;

	const ABuildingSite* BuildingSite = FAIHelper::FindNearestUnfinishedBuilding(Agent.GetFeetPosition());
	if (!BuildingSite)
		return 0;
	
	const float DistanceWeight = FAIHelper::CalculateDistanceWeight(Agent.GetFeetPosition(), BuildingSite->GetActorLocation());
	const float NormalizedPriority = DistanceWeight;
	
	return FMath::GetRangeValue(GoalPriorities::PriorityScaleConstruct, NormalizedPriority); 
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGoalConstructBuilding::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	ABuildingSite* BuildingSite = FAIHelper::FindNearestUnfinishedBuilding(Agent.GetFeetPosition());
	if (!BuildingSite)
		return false;
	
	WorldState.Set(EWorldPropertyKey::Construct, BuildingSite);
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGoalConstructBuilding::DeInit(IAgent& Agent, bool bIsSuccess) const
{
	
}
