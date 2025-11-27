// (c) 2024 by Crenetic GmbH Studios

#include "AI/Actions/ActionGoto.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionGoto::UActionGoto()
{
	Preconditions.Set(EWorldPropertyKey::Harvest, static_cast<UObject*>(nullptr));
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
	check(PropAtNode && PropAtNode->Type == EWorldPropertyType::Object && PropAtNode->Object);

	FTransform Transform;
	FAIHelper::GetObjectTransform(PropAtNode->Object, Transform);

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
	if (!PropAtNode || PropAtNode->Type != EWorldPropertyType::Object || !PropAtNode->Object)
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionGoto::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropAtNode = CurrentWorldState.Get(EWorldPropertyKey::AtNode);
	check(PropAtNode->Type == EWorldPropertyType::Object);
	check(PropAtNode->Object != nullptr);

	if (!Agent.Goto(PropAtNode->Object))
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionGoto::Deactivate(IAgent& Agent, FAIState& AIState) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionGoto::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	if (Agent.HasMovingFailed())
	{
		return EActionResult::Failed;
	}
	if (Agent.HasFinishedMoving())
	{
		return EActionResult::Complete;
	}
	return Super::IsComplete(Agent, AIState);
}
