// (c) 2025 MK


#include "AI/Actions/ActionSatisfyNeedAtInteraction.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Interactive/NeedModifierInteraction.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSatisfyNeedAtInteraction::UActionSatisfyNeedAtInteraction()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, ENodeType::NeedInteraction);
	Results.Set(EWorldPropertyKey::Need, EWorldPropertyKey::Need);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSatisfyNeedAtInteraction::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropNeed = CurrentWorldState.Get(EWorldPropertyKey::Need);
	if (!PropNeed || PropNeed->Type != EWorldPropertyType::NeedType)
		return false;

	if (!FAIHelper::HasNeedInteraction(PropNeed->NeedType))
		return false;
	
	if (!bIsPlanning)
	{
		const ANeedModifierInteraction* NeedInteraction = Agent.GetBlackboard().GetNeedInteraction();
		if (!NeedInteraction)
			return false;
		
		if (NeedInteraction->IsInteracting())
			return false;
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSatisfyNeedAtInteraction::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	ANeedModifierInteraction* NeedInteraction = Agent.GetBlackboard().GetNeedInteraction();
	check(NeedInteraction);
	return Agent.StartInteraction(NeedInteraction);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSatisfyNeedAtInteraction::Deactivate(IAgent& Agent) const
{
	ANeedModifierInteraction* NeedInteraction = Agent.GetBlackboard().GetNeedInteraction();
	check(NeedInteraction);
	Agent.StopInteraction(NeedInteraction);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSatisfyNeedAtInteraction::IsComplete(IAgent& Agent) const
{
	// Interaction will have set the busy timer, so we can just exit here
	return EActionResult::Complete;
}
