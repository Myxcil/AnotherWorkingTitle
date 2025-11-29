// (c) 2025 MK


#include "AI/Actions/ActionConsumeSlot.h"

#include "AI/AIBlackboard.h"
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
	
	if (!bIsPlanning)
	{
		if (!Agent.GetBlackboard().IsSet(EBlackboardMask::SlotIndex))
			return false;
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionConsumeSlot::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	const int32 SlotIndex = Agent.GetBlackboard().GetSlotIndex();
	return Agent.UseSlot(SlotIndex);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionConsumeSlot::Deactivate(IAgent& Agent) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionConsumeSlot::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
