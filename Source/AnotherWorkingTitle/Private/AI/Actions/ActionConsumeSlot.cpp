// (c) 2025 MK


#include "AI/Actions/ActionConsumeSlot.h"

#include "AI/IAgent.h"
#include "Settlers/Needs.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionConsumeSlot::UActionConsumeSlot()
{
	Preconditions.Set(EWorldPropertyKey::HasResource, EWorldPropertyKey::SatisfyNeed);
	Results.Set(EWorldPropertyKey::SatisfyNeed, EWorldPropertyKey::SatisfyNeed);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionConsumeSlot::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropSatisfyNeed = CurrentWorldState.Get(EWorldPropertyKey::SatisfyNeed);
	if (!PropSatisfyNeed || PropSatisfyNeed->Type != EWorldPropertyType::Need)
		return false;
	
	if (!StaticEnum<ENeedType>()->IsValidEnumValue(PropSatisfyNeed->Value))
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionConsumeSlot::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	return Agent.UseSlot(AIState.Index);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionConsumeSlot::Deactivate(IAgent& Agent, FAIState& AIState) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionConsumeSlot::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return EActionResult::Complete;
}
