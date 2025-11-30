// (c) 2024 by Crenetic GmbH Studios


#include "AI/Actions/ActionSearchPath.h"

#include "AI/AIHelper.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSearchPath::UActionSearchPath()
{
	Results.Set(EWorldPropertyKey::PathExists, EWorldPropertyKey::PathExists);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPath::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;

	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Node)
		return false;
	
	if (!FAIHelper::HasValidTransform(Agent, PropAtNode->NodeType, true))
		return false;
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPath::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchPath::Deactivate(IAgent& Agent) const
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSearchPath::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
