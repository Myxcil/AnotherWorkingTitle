// (c) 2024 by Crenetic GmbH Studios


#include "AI/Actions/ActionStay.h"

#include "AI/IAgent.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionStay::UActionStay()
{
	Preconditions.Set(EWorldPropertyKey::Interact, static_cast<UObject*>(nullptr));
	Results.Set(EWorldPropertyKey::AtNode, EWorldPropertyKey::AtNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionStay::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!UAbstractAction::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;

	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Object || !PropAtNode->Object)
		return false;

	if (!Agent.IsNear(PropAtNode->Object))
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionStay::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	return Super::Activate(Agent, AIState, CurrentWorldState);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionStay::Deactivate(IAgent& Agent, FAIState& AIState) const
{
	Super::Deactivate(Agent, AIState);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionStay::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return EActionResult::Complete;
}
