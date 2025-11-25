// (c) 2025 MK


#include "Resources/ResourceNode.h"

#include "Inventory//InventoryComponent.h"
#include "Settlers/SettlerCharacter.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
AResourceNode::AResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AResourceNode::BeginPlay()
{
	Super::BeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AResourceNode::Interact_Implementation(ASettlerCharacter* InstigatorCharacter)
{
	if (!InstigatorCharacter)
		return;
	
	if (UInventoryComponent* InventoryComponent = InstigatorCharacter->GetInventoryComponent())
	{
		const int32 NumHarvested = Harvest(1);
		if (NumHarvested > 0)
		{
			InventoryComponent->AddResource(Resource, NumHarvested);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 AResourceNode::Harvest(const int32 RequestedAmount)
{
	if (!Resource || Amount <= 0 || RequestedAmount <= 0)
		return 0;
	
	const int32 Harvested = FMath::Min(Amount, RequestedAmount);
	Amount -= Harvested;
	return Harvested;
}
