// (c) 2025 MK


#include "AnotherWorkingTitle/Public/Inventory/Inventory.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FInventory::AddResource(const UResourceDefinition* Resource, const int32 Amount)
{
	if (!Resource || Amount <= 0)
		return 0;
	
	const int32 UnitWeight = Resource->UnitWeight;
	int32 AddedTotal = 0;
	
	int32 RemainingAmount = Amount;
	while (RemainingAmount > 0)
	{
		const int32 RemainingCapacity = MaxCarryWeight - GetCurrentWeight();
		if (RemainingCapacity <= 0)
			break;
		
		const int32 MaxByWeight = RemainingCapacity / UnitWeight;
		if (MaxByWeight <= 0)
			break;
		
		const int32 ToAdd = FMath::Min(MaxByWeight, RemainingAmount);
		
		FResourceStack* ExistingStack = Stacks.FindByPredicate([Resource](const FResourceStack& Stack)
		{
			return Stack.Resource == Resource;
		});
		if (ExistingStack)
		{
			ExistingStack->Amount += ToAdd;
		}
		else
		{
			FResourceStack NewStack;
			NewStack.Resource = Resource;
			NewStack.Amount = ToAdd;
			Stacks.Add(MoveTemp(NewStack));
		}
		
		RemainingAmount -= ToAdd;
		AddedTotal += ToAdd;
	}
	return AddedTotal;
}
