// (c) 2025 MK


#include "Inventory/SettlementStock.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FSettlementStock::AddResource(const UResourceDefinition* Resource, const int32 Amount)
{
	check(Resource);
	check(Amount > 0);
	
	int32 RemainingAmount = Amount;
	while (RemainingAmount > 0)
	{
		FResourceStack* ExistingStack = Stacks.FindByPredicate([Resource](const FResourceStack& Stack)
		{
			return Stack.Resource == Resource && Stack.Amount < Stack.Resource->MaxStackSize;
		});
		
		if (!ExistingStack)
		{
			ExistingStack = &Stacks.AddDefaulted_GetRef();
			ExistingStack->Resource = Resource;
			ExistingStack->Amount = 0;
		}
		
		RemainingAmount = ExistingStack->Add(RemainingAmount);
	}
	
	OnGlobalInventoryChanged.Broadcast();
	
	return Amount;
}
