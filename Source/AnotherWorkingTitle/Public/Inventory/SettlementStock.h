// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "InventoryBase.h"
#include "SettlementStock.generated.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FSettlementStock : public FInventoryBase
{
	GENERATED_BODY()

	int32 AddResource(const UResourceDefinition* Resource, const int32 Amount);
};