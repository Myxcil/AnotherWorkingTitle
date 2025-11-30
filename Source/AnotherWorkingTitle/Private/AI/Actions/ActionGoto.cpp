// (c) 2024 by Crenetic GmbH Studios

#include "AI/Actions/ActionGoto.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionGoto::UActionGoto()
{
	Preconditions.Set(EWorldPropertyKey::PathExists, EWorldPropertyKey::AtNode);
	Results.Set(EWorldPropertyKey::AtNode, EWorldPropertyKey::AtNode);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 UActionGoto::GetCost(IAgent& Agent, const FWorldState& WorldState) const
{
	float Cost = Super::GetCost(Agent, WorldState);
	return Cost;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float UActionGoto::CalculateRuntimeCost(IAgent& Agent, const FWorldState& WorldState, FVector& PlannedLocation) const
{
	float Cost = Super::CalculateRuntimeCost(Agent, WorldState, PlannedLocation);

	const FWorldProperty* PropAtNode = WorldState.Get(EWorldPropertyKey::AtNode);
	check(PropAtNode && PropAtNode->Type == EWorldPropertyType::Node);

	FTransform Transform;
	FAIHelper::GetObjectTransform(Agent, PropAtNode->NodeType, Transform);

	const FVector Location = Transform.GetLocation();
	float Distance = FVector::Distance(PlannedLocation, Location);

	Cost += Distance;

	PlannedLocation = Location;
	
	return Cost;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionGoto::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!UAbstractAction::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;

	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Node)
		return false;
	
	if (!FAIHelper::HasValidTransform(Agent, PropAtNode->NodeType, false))
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionGoto::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	check(PropAtNode);
	check(PropAtNode->Type == EWorldPropertyType::Node);

	if (!Agent.Goto(PropAtNode->NodeType))
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionGoto::Deactivate(IAgent& Agent) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionGoto::IsComplete(IAgent& Agent) const
{
	if (Agent.HasMovingFailed())
	{
		return EActionResult::Failed;
	}
	if (Agent.HasFinishedMoving())
	{
		return EActionResult::Complete;
	}
	return Super::IsComplete(Agent);
}
