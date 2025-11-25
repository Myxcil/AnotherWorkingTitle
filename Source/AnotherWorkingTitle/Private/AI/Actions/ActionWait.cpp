// (c) 2024 by Crenetic GmbH Studios

#include "AI/Actions/ActionWait.h"

#include "AI/IAgent.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UActionWait::UActionWait()
{
	Results.Set(EWorldPropertyKey::Wait, EWorldPropertyKey::Wait);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionWait::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!UAbstractAction::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;

	const FWorldProperty* wpWait = CurrentWorldState.Get(EWorldPropertyKey::Wait);
	if (wpWait == nullptr)
		return false;

	if (wpWait->Type != EWorldPropertyType::Bool)
		return false;

	if (!wpWait->bValue)
		return false;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionWait::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionWait::Deactivate(IAgent& Agent, FAIState& AIState) const
{
	Super::Deactivate(Agent, AIState);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionWait::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return Super::IsComplete(Agent, AIState);
}
