// (c) 2025 MK


#include "Inventory/InventoryBase.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FInventoryBase::RemoveResource(const UResourceDefinition* Resource, const int32 Amount)
{
	if (!Resource || Amount <= 0)
		return 0;
	
	int32 RemovedTotal = 0;
	int32 RemainingAmount = Amount;
	
	for (int32 I=Stacks.Num()-1; I >= 0; --I)
	{
		FResourceStack& Stack = Stacks[I];
		if (Stack.Resource != Resource)
			continue;
		
		const int32 ToRemove = FMath::Min(Stack.Amount, RemainingAmount);
		Stack.Amount -= ToRemove;
		RemainingAmount -= ToRemove;
		RemovedTotal += ToRemove;
		
		if (Stack.Amount <= 0)
		{
			Stacks.RemoveAt(I);
		}
		
		if (RemainingAmount <= 0)
			break;
	}
	
	return RemovedTotal;
}
