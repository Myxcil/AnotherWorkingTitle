// (c) 2025 MK


#include "AI/Actions/ActionSearchPathByUse.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Interactive/NeedsModifier.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSearchPathByUse::UActionSearchPathByUse()
{
	Results.Set(EWorldPropertyKey::PathExists, EWorldPropertyKey::AtNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPathByUse::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Node)
		return false;
	
	const FWorldProperty* PropUseSlot = CurrentWorldState.Get(EWorldPropertyKey::UseSlot);
	if (!PropUseSlot || PropUseSlot->Type != EWorldPropertyType::NeedType)
		return false;
		
	if (PropAtNode->NodeType == ENodeType::NeedsModifier)
	{
		const ANeedsModifier* NeedsModifier = FAIHelper::FindNeedImprover(PropUseSlot->NeedType);
		if (!NeedsModifier)
			return false;
		
		return true;
	}
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPathByUse::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	const FWorldProperty* PropUseSlot = CurrentWorldState.Get(EWorldPropertyKey::UseSlot);
	
	if (PropAtNode->NodeType == ENodeType::NeedsModifier)
	{
		if (ANeedsModifier* NeedsModifier = FAIHelper::FindNeedImprover(PropUseSlot->NeedType))
		{
			Agent.GetBlackboard().SetNeedsModifier(NeedsModifier);	
			return true;
		}
	}
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchPathByUse::Deactivate(IAgent& Agent) const
{
	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSearchPathByUse::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
