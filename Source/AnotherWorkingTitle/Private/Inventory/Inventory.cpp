// (c) 2025 MK


#include "Inventory/Inventory.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FInventory::CanCarryMore(const UResourceDefinition* Resource, const int32 Amount) const
{
	check(Resource);
	check(Amount > 0);
	
	const int32 FreeSpaceInExistingSlots = Algo::Accumulate(Stacks, 0, [Resource](const int32 Total, const FResourceStack& Stack )
	{
		return Total + (Stack.Resource == Resource ? Stack.Resource->MaxStackSize - Stack.Amount : 0); 
	});
	
	const int32 FreeBySlots = GetNumFreeSlots() * Resource->MaxStackSize;
	
	const int32 TotalFreeSpace = FreeSpaceInExistingSlots + FreeBySlots;
	
	return TotalFreeSpace >= Amount;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FInventory::GetNumUsedSlots() const
{
	return Algo::Accumulate(Stacks, 0, [](const int32 Total, const FResourceStack& Stack )
	{
		return Total + (Stack.Resource ? 1 : 0);
	});
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FInventory::AddResource(const UResourceDefinition* Resource, const int32 Amount)
{
	check(Resource);
	check(Amount > 0);
	
	int32 FreeSlots = GetNumFreeSlots();
	int32 RemainingAmount = Amount;
	while (RemainingAmount > 0)
	{
		FResourceStack* ExistingStack = Stacks.FindByPredicate([Resource](const FResourceStack& Stack)
		{
			return Stack.Resource == Resource && Stack.Amount < Stack.Resource->MaxStackSize;
		});
		
		if (!ExistingStack)
		{
			if (FreeSlots == 0)
				break;
			
			ExistingStack = &Stacks.AddDefaulted_GetRef();
			ExistingStack->Resource = Resource;
			ExistingStack->Amount = 0;
			--FreeSlots;
		}
		
		RemainingAmount = ExistingStack->Add(RemainingAmount);
	}
	
	const int32 AddedTotal = Amount - RemainingAmount;
	if (AddedTotal > 0)
	{
		OnGlobalInventoryChanged.Broadcast();
	}
	
	return AddedTotal;
}
