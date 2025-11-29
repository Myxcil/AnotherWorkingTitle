// (c) 2025 MK


#include "AI/Actions/ActionSearchInventory.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSearchInventory::UActionSearchInventory()
{
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
	if (!PropSatisfyNeed || PropSatisfyNeed->Type != EWorldPropertyType::Need)
		return false;
	
	const int32 SlotIndex = FAIHelper::FindFirstNeedSatisfactionInInventory(Agent, PropSatisfyNeed->NeedType);
	if (SlotIndex == INDEX_NONE)
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSearchInventory::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropSatisfyNeed = CurrentWorldState.Get(EWorldPropertyKey::SatisfyNeed);
	check(PropSatisfyNeed);
	check(PropSatisfyNeed->Type == EWorldPropertyType::Need);
	
	const int32 SlotIndex = FAIHelper::FindBestNeedSatisfactionInInventory(Agent, PropSatisfyNeed->NeedType);
	if (SlotIndex == INDEX_NONE)
		return false;
	
	Agent.GetBlackboard().SetSlotIndex(SlotIndex);
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSearchInventory::Deactivate(IAgent& Agent) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSearchInventory::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
