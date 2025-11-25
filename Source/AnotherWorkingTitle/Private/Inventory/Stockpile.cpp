// (c) 2025 MK


#include "Inventory/Stockpile.h"

#include "Inventory/InventoryComponent.h"
#include "Settlers/SettlerCharacter.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
AStockpile::AStockpile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AStockpile::Interact_Implementation(ASettlerCharacter* InstigatorCharacter)
{
	if (!InstigatorCharacter)
		return;
	
	if (UInventoryComponent* InventoryComponent = InstigatorCharacter->GetInventoryComponent())
	{
		InventoryComponent->TransferAll(SettlementStock);
	}
}
