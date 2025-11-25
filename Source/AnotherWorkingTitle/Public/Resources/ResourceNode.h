// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactive/Interaction.h"
#include "ResourceNode.generated.h"

class UResourceDefinition;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API AResourceNode : public AActor, public IInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	AResourceNode();
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;

public:
	// IInteraction
	virtual void Interact_Implementation(ASettlerCharacter* InstigatorCharacter) override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool HasResources() const { return Resource && Amount > 0; }
	int32 Harvest(const int32 RequestedAmount);
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<const UResourceDefinition> Resource;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int32 Amount = 100;
};
