// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interaction.generated.h"

class ASettlerCharacter;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UInteraction : public UInterface
{
	GENERATED_BODY()
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
class ANOTHERWORKINGTITLE_API IInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact(ASettlerCharacter* InstigatorCharacter);
};
