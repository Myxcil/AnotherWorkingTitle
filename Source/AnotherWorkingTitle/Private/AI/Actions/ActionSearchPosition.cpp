// (c) 2024 by Crenetic GmbH Studios


#include "AI/Actions/ActionSearchPosition.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSearchPosition::UActionSearchPosition()
{
	Results.Set(EWorldPropertyKey::PathExists, EWorldPropertyKey::AtNode);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPosition::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;

	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Node)
		return false;
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPosition::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	check(PropAtNode);
	check(PropAtNode->Type == EWorldPropertyType::Node);
	Agent.SearchNodePosition(PropAtNode->NodeType);
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchPosition::Deactivate(IAgent& Agent, FAIState& AIState) const
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSearchPosition::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	if (Agent.IsSearchDone())
	{
		return EActionResult::Complete;
	}
	return Super::IsComplete(Agent, AIState);
}
