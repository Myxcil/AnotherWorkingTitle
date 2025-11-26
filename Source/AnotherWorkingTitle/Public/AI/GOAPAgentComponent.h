// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "IAgent.h"
#include "Components/ActorComponent.h"
#include "Planning/Planner.h"
#include "Planning/WorldState.h"
#include "Storage/WorkingMemory.h"
#include "GOAPAgentComponent.generated.h"

class ASettlerCharacter;
class UAbstractGoal;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void InitializeAgent();
	void TickGOAP(const float DeltaSeconds);
	void OnMovementComplete(bool bSuccess);
	
	FAIRequestMoveToLocation OnAIRequestMoveToLocation;
	FAIRequestStopMovement OnAIRequestStopMovement;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	// IAgent
	virtual FString GetActorName() const override { return GetOwner()->GetName(); }
	virtual UResourceRegistrySubsystem* GetResourceRegistry() const override;
	
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
	
	virtual bool CanPickup(const UResourceDefinition* Resource, const int32 Amount = 1) const override;
	virtual int32 GetAmountInInventory(const UResourceDefinition* Resource) const override;

	virtual void Interact(IInteraction* Interaction) override;
	virtual bool IsInteractionDone() const override;
	
	virtual bool IsInCriticalState() const override;
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void InitializeGoalsAndActions();
	void ResetPlan(bool bSuccess);	
	
	void InitWorldState(ASettlerCharacter* SettlerCharacter);
	bool ExecutePlan(ASettlerCharacter* SettlerCharacter);
	void EvaluateGoals();
	const UAbstractGoal* ChooseTopGoal();
	void OnGoalFailed(const UAbstractGoal* Goal);

	bool IsBusy(const ASettlerCharacter* SettlerCharacter) const;
	
	void SetDirty();
		
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float DefaultAcceptanceRadius = 50.0f;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	enum class EInternalState
	{
		Idle,
		IdleFailed,
		Planning,
		Executing,
		AwaitApproval,
		Paused,
	};
	
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
	TWeakObjectPtr<ASettlerCharacter> SettlerPtr;
	TWeakObjectPtr<UResourceRegistrySubsystem> ResourceRegistry;
	
	TArray<FGoalInstance> GoalList;
	TStaticArray<TArray<const UAbstractAction*>, WorldPropertyKeyCount> ActionList;
	
	FWorldState WorldState;
	FWorkingMemory Memory;

	FPlanner Planner;
	EInternalState State;
	bool bWorldIsDirty;

	TWeakObjectPtr<const UAbstractGoal> CurrentGoal;
	FWorldState CurrentGoalState;
	
	TArray<FPlanStep> Plan;
	int32 PlanStep = -1;
	bool bActionActivated = false;
	FAIState AIState;

	bool bEQSRunning = false;
	bool bMoveRequestDone = false;
	bool bMoveRequestFailed = false;
	
	float BusyTimer = 0;
	FDelegateHandle HandleGlobalInventoryChanged;
};
