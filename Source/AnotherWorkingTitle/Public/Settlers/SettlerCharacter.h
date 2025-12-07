// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SettlerCharacter.generated.h"

class UGameTimeSubsystem;
class IHoldInteraction;
class UGOAPAgentComponent;
class UInventoryComponent;
class UNeedsComponent;
class USocialComponent; 

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettlerDied);

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Tick(float DeltaTime) override;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool IsBusy() const;
	FVector GetFeetPosition() const;
	UFUNCTION(BlueprintCallable)
	void SetSprinting(const bool bEnable);
	bool IsSprinting() const { return bIsSprinting; }

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UNeedsComponent* GetNeedsComponent() const				{ return NeedsComponent; }
	UInventoryComponent* GetInventoryComponent() const		{ return InventoryComponent; }
	UGOAPAgentComponent* GetGOAPAgentComponent() const		{ return GOAPAgentComponent; }
	USocialComponent* GetSocialComponent() const			{ return SocialComponent; }

	bool UseSlot(const int32 SlotIndex);
	
	bool IsDead() const { return bIsDead; }
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void TryBeginInteract(AActor* Target);
	UFUNCTION(BlueprintCallable)
	void TryEndInteract();
	UFUNCTION(BlueprintCallable)
	void UseItemInSlot(const int32 SlotIndex);

	UPROPERTY(BlueprintAssignable)
	FSettlerDied OnSettlerDied;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void OnDamagedReachedMaximum();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settler|Movement")
	float WalkSpeed = 250.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Settler|Movement")
	float RunSpeed = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNeedsComponent> NeedsComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UGOAPAgentComponent> GOAPAgentComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USocialComponent> SocialComponent;
		
	TWeakObjectPtr<const UGameTimeSubsystem> GameTimeSubsystemPtr;

	bool bIsSprinting = false;
	
	TWeakObjectPtr<AActor> CurrentHoldInteraction;
	
	float BusyTimerAfterUse = 0.0f;
	bool bIsDead = false;
};
