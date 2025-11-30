// (c) 2025 MK


#include "Inventory/InventoryComponent.h"

#include "AnotherWorkingTitle/AnotherWorkingTitle.h"
#include "Inventory/SettlementStock.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UInventoryComponent::AddResource(const UResourceDefinition* Resource, const int32 Amount)
{
	const int32 NumAdded = Inventory.AddResource(Resource, Amount);
	if (NumAdded > 0)
	{
		OnInventoryChanged.Broadcast();
	}
	UE_LOG(LogAWT, Log, TEXT("%s inventory add %s (%dx) = %d"), *GetOwner()->GetName(), *Resource->Id.ToString(), Amount, NumAdded);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UInventoryComponent::RemoveResource(const int32 SlotIndex, const int32 Amount)
{
	check(Inventory.Stacks.IsValidIndex(SlotIndex));
	const int32 NumRemoved = Inventory.RemoveResourceAtIndex(SlotIndex, Amount);
	if (NumRemoved > 0)
	{
		OnInventoryChanged.Broadcast();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UInventoryComponent::TransferAll(FSettlementStock& Destination)
{
	if (Inventory.Stacks.IsEmpty())
		return;
	
	for (const FResourceStack& Stack : Inventory.Stacks)
	{
		if (Stack.IsValid())
		{
			Destination.AddResource(Stack.Resource, Stack.Amount);
			UE_LOG(LogAWT, Log, TEXT("%s inventory transfer %s (%dx)"), *GetOwner()->GetName(), *Stack.Resource->Id.ToString(), Stack.Amount);
		}
	}
	Inventory.Clear();
	OnInventoryChanged.Broadcast();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UInventoryComponent::TransferByCategory(FSettlementStock& Destination, EResourceCategory ResourceCategory)
{
	if (Inventory.Stacks.IsEmpty())
		return;
	
	for (const FResourceStack& Stack : Inventory.Stacks)
	{
		if (Stack.IsValid() && Stack.Resource->Category == ResourceCategory)
		{
			Destination.AddResource(Stack.Resource, Stack.Amount);
			UE_LOG(LogAWT, Log, TEXT("%s inventory transfer %s (%dx)"), *GetOwner()->GetName(), *Stack.Resource->Id.ToString(), Stack.Amount);
		}
	}
	Inventory.ClearByCategory(ResourceCategory);
	OnInventoryChanged.Broadcast();
}

#if WITH_EDITOR

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UInventoryComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FResourceStack, Amount) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(FResourceStack, Resource))
	{
		for (FResourceStack& Stack : Inventory.Stacks)
		{
			Stack.PostEditChangeProperty(PropertyChangedEvent);
		}
	}
}

#endif
