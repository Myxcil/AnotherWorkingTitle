// (c) 2025 MK


#include "AI/Actions/ActionSearchResourceNode.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Resources/ResourceNode.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSearchResourceNode::UActionSearchResourceNode()
{
	Results.Set(EWorldPropertyKey::HasResource, EWorldPropertyKey::HasResource);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchResourceNode::ApplyPreconditions(IAgent& Agent, FWorldState& GoalState) const
{
	Super::ApplyPreconditions(Agent, GoalState);

	const FWorldProperty* PropSatisfyNeed = GoalState.Get(EWorldPropertyKey::SatisfyNeed);
	check(PropSatisfyNeed)
	check(PropSatisfyNeed->Type == EWorldPropertyType::Need);

	AResourceNode* ResourceNode = FAIHelper::FindNearestResourceNodeByNeed(Agent.GetFeetPosition(), PropSatisfyNeed->NeedType);
	check(ResourceNode);
	
	GoalState.Set(EWorldPropertyKey::Harvest, ResourceNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchResourceNode::ArePreconditionsSatisfied(const FWorldState& CurrentWorldState, const FWorldState& GoalState, EWorldPropertyKey& FailedKey) const
{
	if (!Super::ArePreconditionsSatisfied(CurrentWorldState, GoalState, FailedKey))
		return false;
		
	const FWorldProperty* PropSatisfyNeed = GoalState.Get(EWorldPropertyKey::SatisfyNeed);
	check(PropSatisfyNeed)
	check(PropSatisfyNeed->Type == EWorldPropertyType::Need);
	
	const FWorldProperty* PropHarvest = CurrentWorldState.Get(EWorldPropertyKey::Harvest);
	if (!PropHarvest || PropHarvest->Type != EWorldPropertyType::Object || !PropHarvest->Object)
	{
		FailedKey = EWorldPropertyKey::Harvest;
		return false;
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchResourceNode::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropSatisfyNeed = CurrentWorldState.Get(EWorldPropertyKey::SatisfyNeed);
	if (!PropSatisfyNeed || PropSatisfyNeed->Type != EWorldPropertyType::Need)
		return false;

	if (bIsPlanning)
	{
		const AResourceNode* ResourceNode = FAIHelper::FindNearestResourceNodeByNeed(Agent.GetFeetPosition(), PropSatisfyNeed->NeedType);
		if (!ResourceNode)
			return false;
	}
	else 
	{
		const FWorldProperty* PropHarvest = CurrentWorldState.Get(EWorldPropertyKey::Harvest);
		if (!PropHarvest || PropHarvest->Type != EWorldPropertyType::Object || !PropHarvest)
			return false;
		
		const AResourceNode* ResourceNode = Cast<AResourceNode>(PropHarvest->Object);
		if (!ResourceNode)
			return false;
		
		return true;
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchResourceNode::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropHarvest = CurrentWorldState.Get(EWorldPropertyKey::Harvest);
	check(PropHarvest);
	check(PropHarvest->Type == EWorldPropertyType::Object);
	check(PropHarvest->Object);

	const AResourceNode* ResourceNode = Cast<AResourceNode>(PropHarvest->Object);
	check(ResourceNode);
	const UResourceDefinition* Resource = ResourceNode->GetResource();
	check(Resource);
	
	const int32 SlotIndex = Agent.FindResourceSlot(Resource);
	if (SlotIndex == INDEX_NONE)
		return false;
	
	AIState.Index = SlotIndex;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchResourceNode::Deactivate(IAgent& Agent, FAIState& AIState) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSearchResourceNode::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return EActionResult::Complete;
}
