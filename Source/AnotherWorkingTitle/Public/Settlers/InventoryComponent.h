// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Inventory.h"
#include "InventoryComponent.generated.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UInventoryComponent();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	const FInventory& GetInventory() const { return Inventory; }
	FInventory& GetInventoryMutable() { return Inventory; }
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(AllowPrivateAccess=true))
	FInventory Inventory;
};
