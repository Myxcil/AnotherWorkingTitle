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
	
	bool IsValid() const { return Resource != nullptr && Amount > 0; }
	
	int32 Add(const int32 AmountToAdd)
	{
		check(Resource);
		const int32 MaxToAdd = FMath::Min(AmountToAdd, Resource->MaxStackSize - Amount);
		Amount += MaxToAdd;
		return AmountToAdd - MaxToAdd;
	}
	
	static const FResourceStack Invalid;
	
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif
};
