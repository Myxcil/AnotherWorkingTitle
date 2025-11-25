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
	if (NumAdded == Amount)
	{
		OnInventoryChanged.Broadcast(Inventory);
	}
	UE_LOG(LogAWT, Log, TEXT("%s inventory add %s (%dx) = %d"), *GetOwner()->GetName(), *Resource->Id.ToString(), Amount, NumAdded);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UInventoryComponent::TransferAll(FSettlementStock& Destination)
{
	for (const FResourceStack& Stack : Inventory.Stacks)
	{
		if (Stack.IsValid())
		{
			Destination.AddResource(Stack.Resource, Stack.Amount);
			UE_LOG(LogAWT, Log, TEXT("%s inventory transfer %s (%dx)"), *GetOwner()->GetName(), *Stack.Resource->Id.ToString(), Stack.Amount);
		}
	}
	Inventory.Clear();
	OnInventoryChanged.Broadcast(Inventory);
}
