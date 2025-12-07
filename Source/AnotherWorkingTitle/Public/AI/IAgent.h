// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AIConstants.h"
#include "Actions/AbstractAction.h"
#include "Inventory/Stockpile.h"
#include "Settlers/Needs.h"

class FAIBlackboard;
class AResourceNode;
class IInteraction;
class UResourceDefinition;
class UResourceRegistrySubsystem;
class UAbstractAction;
class FWorldState;
class ANeedModifierInteraction;

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
	virtual FAIBlackboard& GetBlackboard() const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual const FWorldState& GetWorldState() const = 0;
	virtual const TArray<const UAbstractAction*>& GetActions(EWorldPropertyKey Key) const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool HasMood(const uint32 MoodFlag) const = 0;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void SetSprinting(const bool bEnable) = 0;
	virtual bool IsNear(const ENodeType NodeType) const = 0;
	virtual FVector GetFeetPosition() const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Goto(const ENodeType NodeType) = 0;
	virtual void Stop() = 0;
	virtual bool HasMovingFailed() const = 0;
	virtual bool HasFinishedMoving() const = 0;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool CanPickup(const UResourceDefinition* Resource, const int32 Amount = 1) const = 0;
	virtual int32 GetTotalAmountInInventory() const = 0;
	virtual int32 GetTotalAmountInInventoryByCategory(const EResourceCategory ResourceCategory) const = 0;
	virtual int32 FindResourceSlot(const UResourceDefinition* Resource) const = 0;
	virtual int32 GetAmountInInventory(const UResourceDefinition* Resource) const = 0;
	virtual int32 GetNumInventorySlots() const = 0;
	virtual const FResourceStack& GetInventorySlot(const int32 SlotIndex) const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual bool Harvest(AResourceNode* ResourceNode) = 0;
	virtual void DepositAll(AStockpile* Stockpile) = 0;
	virtual void DepositByCategory(AStockpile* Stockpile, const EResourceCategory ResourceCategory) = 0;
	
	virtual bool UseSlot(const int32 SlotIndex) = 0;
	
	virtual bool StartInteraction(ANeedModifierInteraction* Interaction) = 0;
	virtual void StopInteraction(ANeedModifierInteraction* Interaction) = 0;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual ENeedSeverity GetNeedSeverity(const ENeedType NeedType) const = 0;
	virtual float GetNeedValue(const ENeedType NeedType) const = 0;
};
