// (c) 2025 MK


#include "AI/Goals/AbstractGoalCollectResource.h"

#include "AI/AIBlackboard.h"
#include "AI/AIGoalMapping.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "AI/Planning/WorldState.h"
#include "Resources/ResourceNode.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
float UAbstractGoalCollectResource::Evaluate(IAgent& Agent) const
{
	if (Agent.HasMood(MOOD_3(EAgentMood::Stressed, EAgentMood::Afraid, EAgentMood::Angry)))
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
	if (Scarcity == 0)
		return 0;
	
	const float RestedValue = 1.0f - Agent.GetNeedValue(ENeedType::Fatigue);
	const float AlreadyStored = 1.0f - FAIHelper::Clamp01(static_cast<float>(Agent.GetAmountInInventory(Resource)) / Resource->MaxStackSize);
	const float DistanceWeight = FAIHelper::CalculateDistanceWeight(Agent.GetFeetPosition(), ResourceNode->GetActorLocation());
	const float NormalizedPriority = RestedValue * Scarcity * AlreadyStored * DistanceWeight;
		
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
	
	Agent.GetBlackboard().SetResourceNode(ResourceNode);
	
	WorldState.Set(EWorldPropertyKey::HasResource, ENodeType::ResourceNode);
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractGoalCollectResource::DeInit(IAgent& Agent, bool bIsSuccess) const
{
	Agent.GetBlackboard().Clear(EBlackboardMask::ResourceNode);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString UAbstractGoalCollectResource::Describe(IAgent& Agent, const FWorldState& WorldState) const
{
	const UResourceDefinition* Resource = GetResource(Agent);
	check(Resource);
	return FString::Printf(TEXT("harvesting %s"), *Resource->DisplayName.ToString());
}
