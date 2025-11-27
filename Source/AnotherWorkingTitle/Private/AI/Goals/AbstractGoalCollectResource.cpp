// (c) 2025 MK


#include "AI/Goals/AbstractGoalCollectResource.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "AI/Planning/WorldState.h"
#include "Resources/ResourceNode.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
float UAbstractGoalCollectResource::Evaluate(IAgent& Agent) const
{
	if (Agent.IsInCriticalState())
		return 0;
	
	const UResourceDefinition* Resource = GetResource(Agent);
	if (!Resource)
		return 0;
	
	const AResourceNode* ResourceNode = FAIHelper::FindNearestResourceNode(Agent.GetFeetPosition(), Resource);
	if (!ResourceNode)
		return 0;

	if (!Agent.CanPickup(Resource))
		return 0;
	
	const float Scarcity = FAIHelper::CalculateResourceScarcity(Resource);
	const float DistanceWeight = FAIHelper::CalculateDistanceWeight(Agent.GetFeetPosition(), ResourceNode->GetActorLocation());
	const float NormalizedPriority = Scarcity * DistanceWeight;
		
	return FMath::GetRangeValue(GoalPriorities::PriorityScaleCollect, NormalizedPriority);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UAbstractGoalCollectResource::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	const UResourceDefinition* Resource = GetResource(Agent);
	check(Resource);
	
	AResourceNode* ResourceNode = FAIHelper::FindNearestResourceNode(Agent.GetFeetPosition(), Resource);
	if (!ResourceNode)
		return false;
	
	if (!Agent.CanPickup(Resource))
		return false;
	
	WorldState.Set(EWorldPropertyKey::Harvest, ResourceNode);
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractGoalCollectResource::DeInit(IAgent& Agent, bool bIsSuccess) const
{
}
