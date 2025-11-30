// (c) 2025 MK


#include "AI/Actions/ActionHarvest.h"

#include "AI/AIBlackboard.h"
#include "AI/IAgent.h"
#include "Resources/ResourceNode.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionHarvest::UActionHarvest()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, EWorldPropertyKey::HasResource);
	Results.Set(EWorldPropertyKey::HasResource, EWorldPropertyKey::HasResource);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionHarvest::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropHasResource = CurrentWorldState.Get(EWorldPropertyKey::HasResource);
	if (!PropHasResource || PropHasResource->Type != EWorldPropertyType::Node)
		return false;
	
	if (!bIsPlanning)
	{
		const AResourceNode* ResourceNode = Agent.GetBlackboard().GetResourceNode();
		if (!ResourceNode)
			return false;
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionHarvest::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	AResourceNode* ResourceNode = Agent.GetBlackboard().GetResourceNode();
	return Agent.Harvest(ResourceNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionHarvest::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}

