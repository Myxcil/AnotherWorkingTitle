// (c) 2025 MK


#include "AI/Actions/ActionSatisfyNeedAtModifier.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Interactive/NeedModifierArea.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSatisfyNeedAtModifier::UActionSatisfyNeedAtModifier()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, ENodeType::NeedsModifier);
	Results.Set(EWorldPropertyKey::Need, EWorldPropertyKey::Need);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSatisfyNeedAtModifier::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropNeed = CurrentWorldState.Get(EWorldPropertyKey::Need);
	if (!PropNeed || PropNeed->Type != EWorldPropertyType::NeedType)
		return false;
	
	if (!FAIHelper::HasNeedImprover(PropNeed->NeedType))
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSatisfyNeedAtModifier::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSatisfyNeedAtModifier::Deactivate(IAgent& Agent) const
{
	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSatisfyNeedAtModifier::IsComplete(IAgent& Agent) const
{
	if (const ANeedModifierArea* NeedsModifier = Agent.GetBlackboard().GetNeedsModifier())
	{
		const ENeedType NeedType = NeedsModifier->GetAffectedType();
		if (Agent.GetNeedValue(NeedType) > 0)
		{
			return EActionResult::Incomplete;
		}
	}
	return EActionResult::Complete;
}
