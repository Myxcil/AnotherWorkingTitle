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
	int32 MaxStacks = 10;
	
	bool CanCarryMore(const UResourceDefinition* Resource, const int32 Amount) const;
	int32 GetNumUsedSlots() const;
	int32 GetNumFreeSlots() const { return MaxStacks - GetNumUsedSlots(); }
	
	int32 AddResource(const UResourceDefinition* Resource, const int32 Amount);
};