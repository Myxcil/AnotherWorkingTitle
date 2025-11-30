// (c) 2025 MK


#include "Inventory/InventoryBase.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FGlobalInventoryChanged FInventoryBase::OnGlobalInventoryChanged;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FInventoryBase::SanityCheck() const
{
	for (const FResourceStack& Stack : Stacks)
	{
		check(Stack.IsValid());
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FInventoryBase::RemoveResource(const UResourceDefinition* Resource, const int32 Amount)
{
	check(Resource);
	check(Amount > 0);
	
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
	
	if (RemovedTotal != 0)
	{
		OnGlobalInventoryChanged.Broadcast();
	}
	
	return RemovedTotal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FInventoryBase::RemoveResourceAtIndex(const int32 SlotIndex, const int32 Amount)
{
	check(Stacks.IsValidIndex(SlotIndex));
	check(Amount > 0);
	
	FResourceStack& Stack = Stacks[SlotIndex];
	
	const int32 ToRemove = FMath::Min(Stack.Amount, Amount);
	Stack.Amount -= ToRemove;

	if (Stack.Amount == 0)
	{
		Stacks.RemoveAt(SlotIndex);
	}
	
	OnGlobalInventoryChanged.Broadcast();
	
	return ToRemove;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FInventoryBase::Collect(TMap<const UResourceDefinition*, int32>& ResourceMap) const
{
	for (int32 I=0; I < Stacks.Num(); ++I)
	{
		const FResourceStack& Stack = Stacks[I];
		int32& Total = ResourceMap.FindOrAdd(Stack.Resource);
		Total += Stack.Amount;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FInventoryBase::Collect(TArray<FResourceStack>& ResourceStacks) const
{
	for (int32 I=0; I < Stacks.Num(); ++I)
	{
		const FResourceStack& Stack = Stacks[I];
		const UResourceDefinition* Resource = Stack.Resource;
		const int32 Index = ResourceStacks.IndexOfByPredicate([Resource](const FResourceStack& Element)
		{
			return Resource == Element.Resource;
		});
		if (Index == INDEX_NONE)
		{
			ResourceStacks.Add(Stack);
		}
		else
		{
			ResourceStacks[Index].Amount += Stack.Amount;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FInventoryBase::Clear()
{
	Stacks.Reset();
	OnGlobalInventoryChanged.Broadcast();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FInventoryBase::ClearByCategory(const EResourceCategory ResourceCategory)
{
	bool bChanged = false;
	for (int32 I=Stacks.Num()-1; I >= 0; --I)
	{
		const FResourceStack& Stack = Stacks[I];
		check(Stack.IsValid());
		if (Stack.Resource->Category == ResourceCategory)
		{
			Stacks.RemoveAt(I);
			bChanged = true;
		}
	}
	if (bChanged)
	{
		OnGlobalInventoryChanged.Broadcast();
	}
}

#if WITH_EDITOR

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FInventoryBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	for (FResourceStack& Stack : Stacks)
	{
		Stack.PostEditChangeProperty(PropertyChangedEvent);
	}
}

#endif
