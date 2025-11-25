// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "ResourceStack.h"
#include "GameFramework/Actor.h"
#include "Interactive/Interaction.h"
#include "ResourceNode.generated.h"

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
	bool HasResources() const { return Stack.Resource && Stack.Amount > 0; }
	int32 Harvest(const int32 RequestedAmount);
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FResourceStack Stack;
};
