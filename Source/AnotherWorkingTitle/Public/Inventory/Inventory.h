// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "InventoryBase.h"
#include "Inventory.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FInventory : public FInventoryBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCarryWeight = 10;
	
	int32 GetCurrentWeight() const
	{
		return Algo::Accumulate(Stacks, 0, [](const int32 Weight, const FResourceStack& Stack) { return Weight+Stack.GetTotalWeight(); });
	}
	
	bool CanCarryMore(const int32 AdditionalWeight) const
	{
		return GetCurrentWeight() + AdditionalWeight <= MaxCarryWeight;
	}
	
	int32 AddResource(const UResourceDefinition* Resource, const int32 Amount);
};