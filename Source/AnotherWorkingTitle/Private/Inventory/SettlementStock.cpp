// (c) 2025 MK


#include "Inventory/SettlementStock.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FSettlementStock::AddResource(const UResourceDefinition* Resource, const int32 Amount)
{
	if (!Resource || Amount <= 0)
		return;
	
	FResourceStack* Existing = Stacks.FindByPredicate([Resource](const FResourceStack& Stack)
	{
		return Stack.Resource == Resource;
	});
	if (Existing)
	{
		Existing->Amount += Amount;
	}
	else
	{
		FResourceStack& NewStack = Stacks.AddDefaulted_GetRef();
		NewStack.Resource = Resource;
		NewStack.Amount = Amount;
	}

	OnInventoryChanged.Broadcast();
}
