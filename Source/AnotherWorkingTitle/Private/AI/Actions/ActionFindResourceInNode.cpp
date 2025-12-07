// (c) 2025 MK


#include "AI/Actions/ActionFindResourceInNode.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionFindResourceInNode::UActionFindResourceInNode()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, ENodeType::ResourceNode);
	Results.Set(EWorldPropertyKey::HasResource, EWorldPropertyKey::HasResource);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionFindResourceInNode::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropHasResource = CurrentWorldState.Get(EWorldPropertyKey::HasResource);
	if (!PropHasResource)
		return false;
	
	if (PropHasResource->Type == EWorldPropertyType::NeedType)
	{
		if (!FAIHelper::HasResourceNodeByNeed(PropHasResource->NeedType))
			return false;
		
		if (!bIsPlanning)
		{
			if (!Agent.GetBlackboard().IsSet(EBlackboardMask::ResourceNode))
				return false;
		}
		
		return true;
	}
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionFindResourceInNode::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	AResourceNode* ResourceNode = Agent.GetBlackboard().GetResourceNode();
	return Agent.Harvest(ResourceNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionFindResourceInNode::Deactivate(IAgent& Agent) const
{
	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionFindResourceInNode::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
