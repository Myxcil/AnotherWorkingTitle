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
	
	if (PropHasResource->Type == EWorldPropertyType::NeedType)
	{
		const int32 SlotIndex = FAIHelper::FindFirstInInventoryByNeedChange(Agent, PropHasResource->NeedType);
		if (SlotIndex == INDEX_NONE)
			return false;
		
		return true;
	}
	if (PropHasResource->Type == EWorldPropertyType::ResourceCategory)
	{
		const int32 SlotIndex = FAIHelper::FindFirstInInventoryByCategory(Agent, PropHasResource->ResourceCategory);
		if (SlotIndex == INDEX_NONE)
			return false;
		
		return true;
	}
	if (PropHasResource->Type == EWorldPropertyType::Int)
	{
		const int32 SlotIndex = FAIHelper::FindFirstInInventoryByRuntimeId(Agent, PropHasResource->Value);
		if (SlotIndex == INDEX_NONE)
			return false;
		
		return true;
	}
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionFindResourceInInventory::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	return true;
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
