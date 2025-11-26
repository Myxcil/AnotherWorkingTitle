// (c) 2025 MK


#include "Resources/ResourceNode.h"

#include "Inventory//InventoryComponent.h"
#include "Settlers/SettlerCharacter.h"

namespace 
{
	TArray<AResourceNode*> AllResourceNodes;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------
const TArray<AResourceNode*>& AResourceNode::GetInstances()
{
	return AllResourceNodes;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
AResourceNode::AResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AResourceNode::BeginPlay()
{
	Super::BeginPlay();
	AllResourceNodes.Add(this);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AResourceNode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AllResourceNodes.Remove(this);
	Super::EndPlay(EndPlayReason);
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
			InventoryComponent->AddResource(Stack.Resource, NumHarvested);
			if (Stack.Amount == 0)
			{
				OnResourceDepleted.Broadcast();			
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 AResourceNode::Harvest(const int32 RequestedAmount)
{
	if (!Stack.Resource || Stack.Amount <= 0 || RequestedAmount <= 0)
		return 0;
	
	const int32 Harvested = FMath::Min(Stack.Amount, RequestedAmount);
	Stack.Amount -= Harvested;
	return Harvested;
}
