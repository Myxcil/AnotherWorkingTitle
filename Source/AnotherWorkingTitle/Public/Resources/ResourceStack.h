// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "ResourceDefinition.h"
#include "ResourceStack.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FResourceStack
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<const UResourceDefinition> Resource = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 0;
	
	int32 GetTotalWeight() const { return Resource ? Resource->UnitWeight * Amount : 0; }
	bool IsValid() const { return Resource && Amount > 0; }
};
