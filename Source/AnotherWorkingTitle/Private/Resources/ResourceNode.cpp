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
	bIsUnlimited = Stack.Amount <= 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AResourceNode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AllResourceNodes.Remove(this);
	Super::EndPlay(EndPlayReason);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AResourceNode::Harvest(ASettlerCharacter* InstigatorCharacter)
{
	check(Stack.Resource);
		
	if (!InstigatorCharacter)
		return;

	if (UInventoryComponent* InventoryComponent = InstigatorCharacter->GetInventoryComponent())
	{
		constexpr int32 NumToHarvest = 1;

		const FInventory& Inventory = InventoryComponent->GetInventory();
		if (Inventory.CanCarryMore(Stack.Resource, NumToHarvest))
		{
			const int32 NumHarvested = RemoveAmountFromStack(NumToHarvest);
			if (NumHarvested > 0)
			{
				InventoryComponent->AddResource(Stack.Resource, NumHarvested);
				if (!bIsUnlimited && Stack.Amount == 0)
				{
					OnResourceDepleted.Broadcast();			
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AResourceNode::Interact_Implementation(ASettlerCharacter* InstigatorCharacter)
{
	Harvest(InstigatorCharacter);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 AResourceNode::RemoveAmountFromStack(const int32 RequestedAmount)
{
	check(Stack.Resource);
	if (bIsUnlimited)
	{
		return RequestedAmount;
	}
	else
	{
		const int32 Harvested = FMath::Min(Stack.Amount, RequestedAmount);
		Stack.Amount -= Harvested;
		return Harvested;
	}
}
