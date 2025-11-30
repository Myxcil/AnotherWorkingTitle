// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AnotherWorkingTitle/Public/Resources/ResourceStack.h"
#include "InventoryBase.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE(FGlobalInventoryChanged)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FInventoryBase
{
	GENERATED_BODY()
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FResourceStack> Stacks;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	int32 FindFirstSlot(const UResourceDefinition* Resource) const
	{
		for (int32 I=0; I < Stacks.Num(); ++I)
		{
			const FResourceStack& Stack = Stacks[I];
			check(Stack.IsValid());
			if (Stack.Resource == Resource)
			{
				return I;
			}
		}
		return INDEX_NONE;
	}
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	int32 GetTotalAmount() const
	{
		return Algo::Accumulate(Stacks, 0, [](const int32 Total, const FResourceStack& Stack )
		{
			check(Stack.Resource);
			return Total + Stack.Amount;
		});
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	int32 GetAmount(const UResourceDefinition* Resource) const
	{
		check(Resource);
		return Algo::Accumulate(Stacks, 0, [Resource](const int32 Total, const FResourceStack& Stack )
		{
			check(Stack.Resource);
			return Total + (Stack.Resource == Resource ? Stack.Amount : 0);
		});
	}
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	int32 GetAmountByCategory(const EResourceCategory ResourceCategory) const
	{
		return Algo::Accumulate(Stacks, 0, [ResourceCategory](const int32 Total, const FResourceStack& Stack )
		{
			check(Stack.Resource);
			return Total + (Stack.Resource->Category == ResourceCategory ? Stack.Amount : 0);
		});
	}
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	int32 RemoveResource(const UResourceDefinition* Resource, const int32 Amount);
	int32 RemoveResourceAtIndex(const int32 SlotIndex, const int32 Amount);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void Collect(TMap<const UResourceDefinition*, int32>& ResourceMap) const;
	void Collect(TArray<FResourceStack>& ResourceStacks) const;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void Clear();
	void ClearByCategory(const EResourceCategory ResourceCategory);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void SanityCheck() const;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static FGlobalInventoryChanged OnGlobalInventoryChanged;
	
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif
};
