// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "IAgent.h"
#include "Components/ActorComponent.h"
#include "Planning/WorldState.h"
#include "GOAPAgentComponent.generated.h"


class UAbstractGoal;
DECLARE_DELEGATE_RetVal_TwoParams(bool, FAIRequestMoveToLocation, const FVector& /*Location*/, const float /*AcceptanceRadius*/);
DECLARE_DELEGATE(FAIRequestStopMovement);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API UGOAPAgentComponent : public UActorComponent, public IAgent
{
private:
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UGOAPAgentComponent();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void InitializeAgent();
	void TickGOAP(const float DeltaSeconds);
	void OnMovementComplete(bool bSuccess);
	void OnActionComplete(bool bSuccess);
	void OnWorldStateChange();
	
	FAIRequestMoveToLocation OnAIRequestMoveToLocation;
	FAIRequestStopMovement OnAIRequestStopMovement;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	// IAgent
	virtual FString GetActorName() const override { return GetOwner()->GetName(); }
	
	virtual const FWorldState& GetWorldState() const override { return WorldState; }
	virtual const TArray<const UAbstractAction*>& GetActions(EWorldPropertyKey Key) const override { return ActionList[static_cast<uint32>(Key)]; }
	
	virtual void SetSprinting(const bool bEnable) override;
	virtual bool IsNear(const UObject* Object) const override;

	virtual FVector GetGroundPosition() const override;

	virtual void SearchNodePosition(const ENodeType Node) override;
	virtual bool IsSearchDone() const override;
	
	virtual bool Goto(UObject* Object) override;
	virtual void Stop() override;
	virtual bool HasMovingFailed() const override;
	virtual bool HasFinishedMoving() const override;

private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void InitializeGoalsAndActions();

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float DefaultAcceptanceRadius = 50.0f;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	struct FGoalInstance
	{
		const UAbstractGoal* Goal;
		float Priority;

		explicit FGoalInstance(const UAbstractGoal* Goal) : Goal(Goal), Priority(0) { }
		FORCEINLINE bool operator==(const FGoalInstance& Other) const { return Goal == Other.Goal; }
	};
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	TWeakObjectPtr<const UAISettings> SettingsPtr;
	TWeakObjectPtr<class ASettlerCharacter> SettlerPtr;
	FWorldState WorldState;
	
	TArray<FGoalInstance> GoalList;
	TStaticArray<TArray<const UAbstractAction*>, WorldPropertyKeyCount> ActionList;
	
	bool bEQSRunning = false;
};
