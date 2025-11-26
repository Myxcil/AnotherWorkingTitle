// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SettlerCharacter.generated.h"

class IHoldInteraction;
class UGOAPAgentComponent;
class UInventoryComponent;
class UNeedsComponent;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API ASettlerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ASettlerCharacter();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Tick(float DeltaTime) override;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool IsBusy() const;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UNeedsComponent* GetNeedsComponent() const				{ return NeedsComponent; }
	UInventoryComponent* GetInventoryComponent() const		{ return InventoryComponent; }
	UGOAPAgentComponent* GetGOAPAgentComponent() const		{ return GOAPAgentComponent; }

	UFUNCTION(BlueprintCallable)
	void TryBeginInteract(AActor* Target);
	UFUNCTION(BlueprintCallable)
	void TryEndInteract();
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UNeedsComponent> NeedsComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UGOAPAgentComponent> GOAPAgentComponent;
	
	TWeakObjectPtr<AActor> CurrentHoldInteraction;
};
