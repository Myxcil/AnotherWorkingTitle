// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "SettlementStock.h"
#include "GameFramework/Actor.h"
#include "Interactive/Interaction.h"
#include "Stockpile.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API AStockpile : public AActor, public IInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static const TArray<AStockpile*>& GetInstances();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	AStockpile();
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	const FSettlementStock& GetSettlementStock() const { return SettlementStock; }
	int32 RemoveResource(const UResourceDefinition* Resource, const int32 Amount);
	
	UFUNCTION(BlueprintCallable)
	void CollectContent(UPARAM(ref) TArray<FResourceStack>& ResourceStacks);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void TransferWholeInventory(ASettlerCharacter* InstigatorCharacter);
	void TransferByCategory(ASettlerCharacter* InstigatorCharacter, const EResourceCategory ResourceCategory);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	// IInteraction
	virtual void Interact_Implementation(ASettlerCharacter* InstigatorCharacter) override;

	UPROPERTY(BlueprintAssignable)
	FInventoryChanged OnInventoryChanged;
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	FSettlementStock SettlementStock;
};
