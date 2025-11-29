// (c) 2025 MK


#include "AI/Actions/ActionSearchInventory.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "Settlers/Needs.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSearchInventory::UActionSearchInventory()
{
	Preconditions.Set(EWorldPropertyKey::SatisfyNeed, EWorldPropertyKey::SatisfyNeed);
	Results.Set(EWorldPropertyKey::HasResource, EWorldPropertyKey::HasResource);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchInventory::ApplyResults(IAgent& Agent, FWorldState& CurrentWorldState, const FWorldState& GoalWorldState) const
{
	Super::ApplyResults(Agent, CurrentWorldState, GoalWorldState);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchInventory::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropSatisfyNeed = CurrentWorldState.Get(EWorldPropertyKey::SatisfyNeed);
	if (!PropSatisfyNeed || PropSatisfyNeed->Type != EWorldPropertyType::Int)
		return false;
	
	if (!StaticEnum<ENeedType>()->IsValidEnumValue(PropSatisfyNeed->Value))
		return false;

	const ENeedType NeedType = static_cast<ENeedType>(PropSatisfyNeed->Value);
	const int32 SlotIndex = FAIHelper::FindBestNeedSatisfactionInInventory(Agent, NeedType);
	if (SlotIndex == INDEX_NONE)
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchInventory::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropSatisfyNeed = CurrentWorldState.Get(EWorldPropertyKey::SatisfyNeed);
	check(PropSatisfyNeed);
	check(PropSatisfyNeed->Type == EWorldPropertyType::Int);
	check(StaticEnum<ENeedType>()->IsValidEnumValue(PropSatisfyNeed->Value));
	
	const ENeedType NeedType = static_cast<ENeedType>(PropSatisfyNeed->Value);
	const int32 SlotIndex = FAIHelper::FindBestNeedSatisfactionInInventory(Agent, NeedType);
	if (SlotIndex == INDEX_NONE)
		return false;
	
	AIState.Index = SlotIndex;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchInventory::Deactivate(IAgent& Agent, FAIState& AIState) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSearchInventory::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return EActionResult::Complete;
}
