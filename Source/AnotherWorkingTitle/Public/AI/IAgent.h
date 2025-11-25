// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AIConstants.h"

class UAbstractAction;
class FWorldState;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class IAgent
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual ~IAgent() = default;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual FString GetActorName() const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual const FWorldState& GetWorldState() const = 0;
	virtual const TArray<const UAbstractAction*>& GetActions(EWorldPropertyKey Key) const = 0;
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void SetSprinting(const bool bEnable) = 0;

	virtual bool IsNear(const UObject* Object) const = 0;
	virtual FVector GetGroundPosition() const = 0;
	
	virtual void SearchNodePosition(const ENodeType Node) = 0;
	virtual bool IsSearchDone() const = 0;
	
	virtual bool Goto(UObject* Object) = 0;
	virtual void Stop() = 0;
	virtual bool HasMovingFailed() const = 0;
	virtual bool HasFinishedMoving() const = 0;
};
