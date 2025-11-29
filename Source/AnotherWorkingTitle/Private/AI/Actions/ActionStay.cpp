// (c) 2024 by Crenetic GmbH Studios


#include "AI/Actions/ActionStay.h"

#include "AI/IAgent.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionStay::UActionStay()
{
	Results.Set(EWorldPropertyKey::AtNode, EWorldPropertyKey::AtNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionStay::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!UAbstractAction::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;

	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Node)
		return false;

	if (!Agent.IsNear(PropAtNode->NodeType))
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionStay::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	return Super::Activate(Agent, CurrentWorldState);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionStay::Deactivate(IAgent& Agent) const
{
	Super::Deactivate(Agent);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionStay::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
