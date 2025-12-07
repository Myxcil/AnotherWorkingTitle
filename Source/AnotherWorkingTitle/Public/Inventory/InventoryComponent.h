// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Stockpile.h"
#include "Components/ActorComponent.h"
#include "Inventory/Inventory.h"
#include "InventoryComponent.generated.h"

struct FSettlementStock;

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
	void RemoveResource(const int32 SlotIndex, const int32 Amount);
	
	void TransferAll(FSettlementStock& Destination);
	void TransferByCategory(FSettlementStock& Destination, EResourceCategory ResourceCategory);

	UPROPERTY(BlueprintAssignable)
	FInventoryChanged OnInventoryChanged;
	
	UFUNCTION(BlueprintPure)
	const FInventory& GetInventory() const { return Inventory; }
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess=true))
	FInventory Inventory;
};
