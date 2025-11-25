// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SettlerCharacter.generated.h"

class UGOAPAgentComponent;
class UWorkComponent;
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
	
	UNeedsComponent* GetNeedsComponent() const			{ return NeedsComponent; }
	UInventoryComponent* GetInventoryComponent() const	{ return InventoryComponent; }
	UWorkComponent* GetWorkComponent() const			{ return WorkComponent; }
	UGOAPAgentComponent* GetGOAPAgentComponent() const	{ return GOAPAgentComponent; }

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UNeedsComponent> NeedsComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UWorkComponent> WorkComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UGOAPAgentComponent> GOAPAgentComponent;
};
