// (c) 2025 MK


#include "AI/Actions/ActionHarvest.h"

#include "AI/IAgent.h"
#include "Resources/ResourceNode.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionHarvest::UActionHarvest()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, EWorldPropertyKey::Harvest);
	Results.Set(EWorldPropertyKey::Harvest, EWorldPropertyKey::Harvest);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionHarvest::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropHarvest = CurrentWorldState.Get(EWorldPropertyKey::Harvest);
	if (!PropHarvest || PropHarvest->Type != EWorldPropertyType::Object || !PropHarvest->Object)
		return false;
	
	const AResourceNode* ResourceNode = Cast<AResourceNode>(PropHarvest->Object);
	if (!ResourceNode)
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionHarvest::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropHarvest = CurrentWorldState.Get(EWorldPropertyKey::Harvest);
	check(PropHarvest);
	check(PropHarvest->Type == EWorldPropertyType::Object);
	check(PropHarvest->Object);
	
	AResourceNode* ResourceNode = Cast<AResourceNode>(PropHarvest->Object);
	return Agent.Harvest(ResourceNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionHarvest::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return EActionResult::Complete;
}

