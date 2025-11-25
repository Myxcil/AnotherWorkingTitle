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
	AStockpile();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	const FSettlementStock& GetSettlementStock() const { return SettlementStock; }
	FSettlementStock& GetSettlementStockMutable() { return SettlementStock; }
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	// IInteraction
	virtual void Interact_Implementation(ASettlerCharacter* InstigatorCharacter) override;

private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	FSettlementStock SettlementStock;
};
