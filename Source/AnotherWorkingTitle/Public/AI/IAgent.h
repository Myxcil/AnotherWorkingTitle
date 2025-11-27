// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AIConstants.h"
#include "Actions/AbstractAction.h"
#include "Inventory/Stockpile.h"

class AResourceNode;
class IInteraction;
class UResourceDefinition;
class UResourceRegistrySubsystem;
class UAbstractAction;
class FWorldState;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
struct FAIState
{
	float Value = 0;
	int32 Index = 0;
	UObject* Object = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class IAgent
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual ~IAgent() = default;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual FString GetActorName() const = 0;
	virtual UResourceRegistrySubsystem* GetResourceRegistry() const = 0;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual const FWorldState& GetWorldState() const = 0;
	virtual const TArray<const UAbstractAction*>& GetActions(EWorldPropertyKey Key) const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void SetSprinting(const bool bEnable) = 0;
	virtual bool IsNear(const UObject* Object) const = 0;
	virtual FVector GetFeetPosition() const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void SearchNodePosition(const ENodeType Node) = 0;
	virtual bool IsSearchDone() const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Goto(UObject* Object) = 0;
	virtual void Stop() = 0;
	virtual bool HasMovingFailed() const = 0;
	virtual bool HasFinishedMoving() const = 0;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool CanPickup(const UResourceDefinition* Resource, const int32 Amount = 1) const = 0;
	virtual int32 GetTotalAmountInInventory() const = 0;
	virtual int32 GetAmountInInventory(const UResourceDefinition* Resource) const = 0;
	virtual float CalculateAccumulatedInventoryValue() const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Harvest(AResourceNode* ResourceNode) = 0;
	virtual void DepositAll(AStockpile* Stockpile) = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool IsInCriticalState() const = 0;
};
