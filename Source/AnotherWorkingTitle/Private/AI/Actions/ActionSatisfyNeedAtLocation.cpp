// (c) 2025 MK


#include "AI/Actions/ActionSatisfyNeedAtLocation.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Interactive/NeedsModifier.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSatisfyNeedAtLocation::UActionSatisfyNeedAtLocation()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, ENodeType::NeedsModifier);
	Results.Set(EWorldPropertyKey::Need, EWorldPropertyKey::Need);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSatisfyNeedAtLocation::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropNeed = CurrentWorldState.Get(EWorldPropertyKey::Need);
	if (!PropNeed || PropNeed->Type != EWorldPropertyType::NeedType)
		return false;
	
	const ANeedsModifier* NeedsModifier = FAIHelper::FindNeedImprover(PropNeed->NeedType);
	if (!NeedsModifier)
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSatisfyNeedAtLocation::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSatisfyNeedAtLocation::Deactivate(IAgent& Agent) const
{
	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSatisfyNeedAtLocation::IsComplete(IAgent& Agent) const
{
	if (const ANeedsModifier* NeedsModifier = Agent.GetBlackboard().GetNeedsModifier())
	{
		const ENeedType NeedType = NeedsModifier->GetAffectedType();
		if (Agent.GetNeedSeverity(NeedType) != ENeedSeverity::Normal)
		{
			return EActionResult::Incomplete;
		}
	}
	return EActionResult::Complete;
}
