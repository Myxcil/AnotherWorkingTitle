// (c) 2025 MK


#include "AI/Actions/ActionFindResourceInInventory.h"

#include "AI/AIBlackboard.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionFindResourceInInventory::UActionFindResourceInInventory()
{
	Results.Set(EWorldPropertyKey::HasResource, EWorldPropertyKey::HasResource);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionFindResourceInInventory::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;

	const FWorldProperty* PropHasResource = CurrentWorldState.Get(EWorldPropertyKey::HasResource);
	if (!PropHasResource)
		return false;
	
	if (PropHasResource->Type == EWorldPropertyType::Need)
	{
		const int32 SlotIndex = FAIHelper::FindFirstNeedSatisfactionInInventory(Agent, PropHasResource->NeedType);
		if (SlotIndex == INDEX_NONE)
			return false;
		
		return true;
	}
		
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionFindResourceInInventory::ApplyPreconditions(IAgent& Agent, FWorldState& GoalState) const
{
	Super::ApplyPreconditions(Agent, GoalState);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionFindResourceInInventory::ArePreconditionsSatisfied(const FWorldState& CurrentWorldState, const FWorldState& GoalState, EWorldPropertyKey& FailedKey) const
{
	if (!Super::ArePreconditionsSatisfied(CurrentWorldState, GoalState, FailedKey))
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionFindResourceInInventory::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	if (const FWorldProperty* PropHasResource = CurrentWorldState.Get(EWorldPropertyKey::HasResource))
	{
		if (PropHasResource->Type == EWorldPropertyType::Need)
		{
			const int32 SlotIndex = FAIHelper::FindBestNeedSatisfactionInInventory(Agent, PropHasResource->NeedType);
			if (SlotIndex == INDEX_NONE)
				return false;
		
			Agent.GetBlackboard().SetSlotIndex(SlotIndex);
		
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionFindResourceInInventory::Deactivate(IAgent& Agent) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionFindResourceInInventory::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
