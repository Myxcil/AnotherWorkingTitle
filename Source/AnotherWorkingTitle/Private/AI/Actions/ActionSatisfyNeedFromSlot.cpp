// (c) 2025 MK


#include "AI/Actions/ActionSatisfyNeedFromSlot.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionSatisfyNeedFromSlot::UActionSatisfyNeedFromSlot()
{
	Preconditions.Set(EWorldPropertyKey::HasResource, EWorldPropertyKey::Need);
	Results.Set(EWorldPropertyKey::Need, EWorldPropertyKey::Need);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSatisfyNeedFromSlot::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropNeed = CurrentWorldState.Get(EWorldPropertyKey::Need);
	if (!PropNeed || PropNeed->Type != EWorldPropertyType::NeedType)
		return false;
	
	if (PropNeed->NeedType == ENeedType::Unknown)
		return false;
	
	if (!bIsPlanning)
	{
		const int32 SlotIndex = FAIHelper::FindFirstInInventoryByNeedChange(Agent, PropNeed->NeedType);
		if (SlotIndex == INDEX_NONE)
			return false;
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionSatisfyNeedFromSlot::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropNeed = CurrentWorldState.Get(EWorldPropertyKey::Need);
	check(PropNeed);
	check(PropNeed->Type == EWorldPropertyType::NeedType);

	const int32 SlotIndex = FAIHelper::FindBestInInventoryByNeedChange(Agent, PropNeed->NeedType);
	return Agent.UseSlot(SlotIndex);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionSatisfyNeedFromSlot::Deactivate(IAgent& Agent) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionSatisfyNeedFromSlot::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
