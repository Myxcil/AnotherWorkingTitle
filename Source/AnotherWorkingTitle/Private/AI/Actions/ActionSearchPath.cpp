// (c) 2024 by Crenetic GmbH Studios


#include "AI/Actions/ActionSearchPath.h"

#include "AI/AIHelper.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSearchPath::UActionSearchPath()
{
	Results.Set(EWorldPropertyKey::PathExists, EWorldPropertyKey::AtNode);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPath::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;

	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Object || !PropAtNode->Object)
		return false;

	FTransform Transform;
	if (!FAIHelper::GetObjectTransform(PropAtNode->Object, Transform))
		return false;
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchPath::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchPath::Deactivate(IAgent& Agent, FAIState& AIState) const
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSearchPath::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return EActionResult::Complete;
}
