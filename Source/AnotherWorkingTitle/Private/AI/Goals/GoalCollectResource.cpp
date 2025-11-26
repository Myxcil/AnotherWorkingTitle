// (c) 2025 MK


#include "AI/Goals/GoalCollectResource.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "AI/Planning/WorldState.h"
#include "Resources/ResourceNode.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
float UGoalCollectResource::Evaluate(IAgent& Agent) const
{
	if (Agent.IsInCriticalState())
		return 0;
	
	const UResourceDefinition* Resource = FAIHelper::FindMinResourceInStockpiles(Agent.GetResourceRegistry());
	if (!Resource)
		return 0;
	
	const AResourceNode* ResourceNode = FAIHelper::FindNearestResourceNode(Agent.GetGroundPosition(), Resource);
	if (!ResourceNode)
		return 0;

	if (!Agent.CanPickup(Resource))
		return 0;
	
	return 0.5f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGoalCollectResource::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	const UResourceDefinition* Resource = FAIHelper::FindMinResourceInStockpiles(Agent.GetResourceRegistry());
	if (!Resource)
		return false;
	
	AResourceNode* ResourceNode = FAIHelper::FindNearestResourceNode(Agent.GetGroundPosition(), Resource);
	if (!ResourceNode)
		return false;
	
	if (!Agent.CanPickup(Resource))
		return false;
	
	WorldState.Set(EWorldPropertyKey::Interact, ResourceNode);
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UGoalCollectResource::DeInit(IAgent& Agent, bool bIsSuccess) const
{
}
