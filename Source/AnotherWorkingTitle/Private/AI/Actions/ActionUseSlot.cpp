// (c) 2025 MK


#include "AI/Actions/ActionUseSlot.h"

#include "AI/AIBlackboard.h"
#include "AI/IAgent.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionUseSlot::UActionUseSlot()
{
	Preconditions.Set(EWorldPropertyKey::HasResource, EWorldPropertyKey::UseSlot);
	Results.Set(EWorldPropertyKey::UseSlot, EWorldPropertyKey::UseSlot);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionUseSlot::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropUseSlot = CurrentWorldState.Get(EWorldPropertyKey::UseSlot);
	if (!PropUseSlot)
		return false;
	
	if (!bIsPlanning)
	{
		const int32 SlotIndex = Agent.GetBlackboard().GetSlotIndex();
		if (SlotIndex == INDEX_NONE)
			return false;
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionUseSlot::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	const int32 SlotIndex = Agent.GetBlackboard().GetSlotIndex();
	return Agent.UseSlot(SlotIndex);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionUseSlot::Deactivate(IAgent& Agent) const
{
	Agent.GetBlackboard().Clear(EBlackboardMask::SlotIndex);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionUseSlot::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
