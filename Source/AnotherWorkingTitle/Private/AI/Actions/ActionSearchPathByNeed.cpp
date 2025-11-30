// (c) 2025 MK


#include "AI/Actions/ActionSearchPathByNeed.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Interactive/NeedsModifier.h"

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
		const ANeedsModifier* NeedsModifier = FAIHelper::FindNeedImprover(PropNeed->NeedType);
		if (!NeedsModifier)
			return false;
		
		return true;
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
		if (ANeedsModifier* NeedsModifier = FAIHelper::FindNeedImprover(PropNeed->NeedType))
		{
			Agent.GetBlackboard().SetNeedsModifier(NeedsModifier);	
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
