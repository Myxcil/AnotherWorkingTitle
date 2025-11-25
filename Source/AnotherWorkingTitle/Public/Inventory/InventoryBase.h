// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AnotherWorkingTitle/Public/Resources/ResourceStack.h"
#include "InventoryBase.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FInventoryBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FResourceStack> Stacks;

	int32 GetAmount(const UResourceDefinition* Resource) const
	{
		return Algo::Accumulate(Stacks, 0, [Resource](const int32 Total, const FResourceStack& Stack )
		{
			return Stack.Resource == Resource ? Stack.Amount : 0;
		});
	}

	int32 RemoveResource(const UResourceDefinition* Resource, const int32 Amount);
	
	void Clear()
	{
		Stacks.Reset();
	}
};
