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

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UHoldInteraction : public UInterface
{
	GENERATED_BODY()
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
class ANOTHERWORKINGTITLE_API IHoldInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeginInteraction(ASettlerCharacter* SettlerCharacter);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool TickInteraction(ASettlerCharacter* SettlerCharacter, float DeltaTime);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EndInteraction(ASettlerCharacter* SettlerCharacter);
};
