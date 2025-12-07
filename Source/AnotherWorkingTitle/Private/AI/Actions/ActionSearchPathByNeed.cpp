// (c) 2025 MK


#include "AI/Actions/ActionSearchPathByNeed.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Interactive/NeedModifierArea.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSearchPathByNeed::UActionSearchPathByNeed()
{
	Results.Set(EWorldPropertyKey::PathExists, EWorldPropertyKey::AtNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPathByNeed::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Node)
		return false;
	
	const FWorldProperty* PropNeed = CurrentWorldState.Get(EWorldPropertyKey::Need);
	if (!PropNeed || PropNeed->Type != EWorldPropertyType::NeedType)
		return false;
		
	if (PropAtNode->NodeType == ENodeType::NeedsModifier)
	{
		if (FAIHelper::HasNeedImprover(PropNeed->NeedType))
		{
			return true;
		}
	}
	else if (PropAtNode->NodeType == ENodeType::ResourceNode)
	{
		if (FAIHelper::HasResourceNodeByNeed(PropNeed->NeedType))
		{
			return true;
		}
	}
	else if (PropAtNode->NodeType == ENodeType::NeedInteraction)
	{
		if (FAIHelper::HasNeedInteraction(PropNeed->NeedType))
		{
			return true;
		}
	}
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPathByNeed::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	const FWorldProperty* PropNeed = CurrentWorldState.Get(EWorldPropertyKey::Need);
	
	if (PropAtNode->NodeType == ENodeType::NeedsModifier)
	{
		if (ANeedModifierArea* NeedsModifier = FAIHelper::FindNearestNeedImprover(Agent.GetFeetPosition(), PropNeed->NeedType))
		{
			Agent.GetBlackboard().SetNeedsModifier(NeedsModifier);	
			return true;
		}
	}
	else if (PropAtNode->NodeType == ENodeType::ResourceNode)
	{
		if (AResourceNode* ResourceNode = FAIHelper::FindNearestResourceNodeByNeed(Agent.GetFeetPosition(), PropNeed->NeedType))
		{
			Agent.GetBlackboard().SetResourceNode(ResourceNode);
			return true;
		}
	}
	else if (PropAtNode->NodeType == ENodeType::NeedInteraction)
	{
		if (ANeedModifierInteraction* NeedInteraction = FAIHelper::FindNearestNeedInteraction(Agent.GetFeetPosition(), PropNeed->NeedType))
		{
			Agent.GetBlackboard().SetNeedInteraction(NeedInteraction);
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchPathByNeed::Deactivate(IAgent& Agent) const
{
	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSearchPathByNeed::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
