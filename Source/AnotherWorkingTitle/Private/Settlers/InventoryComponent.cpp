// (c) 2025 MK


#include "Settlers/InventoryComponent.h"


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
