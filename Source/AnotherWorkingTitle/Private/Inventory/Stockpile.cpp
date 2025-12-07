// (c) 2025 MK


#include "Inventory/Stockpile.h"

#include "Inventory/InventoryComponent.h"
#include "Settlers/SettlerCharacter.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace 
{
	TArray<AStockpile*> AllStockpiles;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------
const TArray<AStockpile*>& AStockpile::GetInstances()
{
	return AllStockpiles;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
AStockpile::AStockpile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AStockpile::BeginPlay()
{
	Super::BeginPlay();
	AllStockpiles.Add(this);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AStockpile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AllStockpiles.Remove(this);
	Super::EndPlay(EndPlayReason);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 AStockpile::RemoveResource(const UResourceDefinition* Resource, const int32 Amount)
{
	const int32 NumRemoved = SettlementStock.RemoveResource(Resource, Amount);
	if (NumRemoved > 0)
	{
		OnInventoryChanged.Broadcast();
	}
	return NumRemoved;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AStockpile::CollectContent(TArray<FResourceStack>& ResourceStacks)
{
	SettlementStock.Collect(ResourceStacks);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AStockpile::TransferWholeInventory(ASettlerCharacter* InstigatorCharacter)
{
	if (!InstigatorCharacter)
		return;
	
	if (UInventoryComponent* InventoryComponent = InstigatorCharacter->GetInventoryComponent())
	{
		InventoryComponent->TransferAll(SettlementStock);
		OnInventoryChanged.Broadcast();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AStockpile::TransferByCategory(ASettlerCharacter* InstigatorCharacter, const EResourceCategory ResourceCategory)
{
	if (!InstigatorCharacter)
		return;
	
	if (UInventoryComponent* InventoryComponent = InstigatorCharacter->GetInventoryComponent())
	{
		InventoryComponent->TransferByCategory(SettlementStock, ResourceCategory);
		OnInventoryChanged.Broadcast();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AStockpile::Interact_Implementation(ASettlerCharacter* InstigatorCharacter)
{
	TransferWholeInventory(InstigatorCharacter);
}
