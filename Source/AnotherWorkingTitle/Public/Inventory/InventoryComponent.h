// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Inventory.h"
#include "InventoryComponent.generated.h"

struct FSettlementStock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);

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
	void AddResource(const UResourceDefinition* Resource, const int32 Amount);
	void TransferAll(FSettlementStock& Destination);

	UPROPERTY(BlueprintAssignable)
	FInventoryChanged OnInventoryChanged;
	UFUNCTION(BlueprintPure)
	const FInventory& GetInventory() const { return Inventory; }
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(AllowPrivateAccess=true))
	FInventory Inventory;
};
