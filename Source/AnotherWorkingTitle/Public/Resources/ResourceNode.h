// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "ResourceStack.h"
#include "GameFramework/Actor.h"
#include "Interactive/Interaction.h"
#include "ResourceNode.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResourceDepleted);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API AResourceNode : public AActor, public IInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static const TArray<AResourceNode*>& GetInstances();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	AResourceNode();
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// IInteraction
	virtual void Interact_Implementation(ASettlerCharacter* InstigatorCharacter) override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool HasResource(const UResourceDefinition* InResource) const { return Stack.Resource == InResource && Stack.Amount > 0; }
	int32 Harvest(const int32 RequestedAmount);
	
	const UResourceDefinition* GetResource() const { return Stack.Resource; }
	
	UPROPERTY(BlueprintAssignable)
	FResourceDepleted OnResourceDepleted;
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FResourceStack Stack;
};
