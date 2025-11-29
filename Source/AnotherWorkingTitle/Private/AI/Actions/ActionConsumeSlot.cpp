// (c) 2025 MK


#include "AI/Actions/ActionConsumeSlot.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Settlers/Needs.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionConsumeSlot::UActionConsumeSlot()
{
	Results.Set(EWorldPropertyKey::SatisfyNeed, EWorldPropertyKey::SatisfyNeed);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionConsumeSlot::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropSatisfyNeed = CurrentWorldState.Get(EWorldPropertyKey::SatisfyNeed);
	if (!PropSatisfyNeed || PropSatisfyNeed->Type != EWorldPropertyType::Int)
		return false;
	
	if (!StaticEnum<ENeedType>()->IsValidEnumValue(PropSatisfyNeed->Value))
		return false;
	
	const ENeedType NeedType = static_cast<ENeedType>(PropSatisfyNeed->Value);
	const int32 Slot = FAIHelper::FindBestNeedSatisfactionInInventory(Agent, NeedType);
	if (Slot == INDEX_NONE)
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionConsumeSlot::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropSatisfyNeed = CurrentWorldState.Get(EWorldPropertyKey::SatisfyNeed);
	check(PropSatisfyNeed);
	check(PropSatisfyNeed->Type == EWorldPropertyType::Int);
	check(StaticEnum<ENeedType>()->IsValidEnumValue(PropSatisfyNeed->Value));
	
	const ENeedType NeedType = static_cast<ENeedType>(PropSatisfyNeed->Value);
	const int32 Slot = FAIHelper::FindBestNeedSatisfactionInInventory(Agent, NeedType);
	if (Slot == INDEX_NONE)
		return false;
	
	return Agent.UseSlot(Slot);
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
