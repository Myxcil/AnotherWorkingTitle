// (c) 2025 MK


#include "AI/Actions/ActionInteract.h"

#include "AI/IAgent.h"
#include "Interactive/Interaction.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionInteract::UActionInteract()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, EWorldPropertyKey::Interact);
	Results.Set(EWorldPropertyKey::Interact, EWorldPropertyKey::Interact);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionInteract::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropInteract = CurrentWorldState.Get(EWorldPropertyKey::Interact);
	if (!PropInteract || PropInteract->Type != EWorldPropertyType::Object || !PropInteract->Object)
		return false;
	
	const IInteraction* Interaction = Cast<IInteraction>(PropInteract->Object);
	if (!Interaction)
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionInteract::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropInteract = CurrentWorldState.Get(EWorldPropertyKey::Interact);
	check(PropInteract);
	check(PropInteract->Type == EWorldPropertyType::Object);
	check(PropInteract->Object);
	
	IInteraction* Interaction = Cast<IInteraction>(PropInteract->Object);
	Agent.Interact(Interaction);
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionInteract::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return Agent.IsInteractionDone() ? EActionResult::Complete : EActionResult::Incomplete;
}

