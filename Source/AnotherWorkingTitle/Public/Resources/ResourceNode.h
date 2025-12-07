// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "ResourceStack.h"
#include "Interactive/Interaction.h"
#include "Interactive/InteractiveActor.h"
#include "ResourceNode.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE(FGlobalResourceNodeChanged)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResourceDepleted);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API AResourceNode : public AInteractiveActor, public IInteraction
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
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool CanHarvest() const { return bIsUnlimited || Stack.Amount > 0; }
	bool Harvest(ASettlerCharacter* InstigatorCharacter);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	// IInteraction
	virtual void Interact_Implementation(ASettlerCharacter* InstigatorCharacter) override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool HasResource(const UResourceDefinition* InResource) const { return Stack.Resource == InResource && CanHarvest(); }
	const UResourceDefinition* GetResource() const { return Stack.Resource; }
	
	UPROPERTY(BlueprintAssignable)
	FResourceDepleted OnResourceDepleted;
	
	static FGlobalResourceNodeChanged OnGlobalResourceNodeChanged;
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	int32 RemoveAmountFromStack(const int32 RequestedAmount);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FResourceStack Stack;
	
	bool bIsUnlimited = false;
};
