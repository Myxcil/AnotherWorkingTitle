// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AIBlackboard.h"
#include "IAgent.h"
#include "Components/ActorComponent.h"
#include "Planning/Planner.h"
#include "Planning/WorldState.h"
#include "Settlers/Emotions.h"
#include "Storage/WorkingMemory.h"
#include "GOAPAgentComponent.generated.h"

class ASettlerCharacter;
class UAbstractGoal;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FAIRequestMoveToLocation, const FVector& /*Location*/, const float /*LocationThreshold*/, const float /*AcceptanceRadius*/);
DECLARE_DELEGATE(FAIRequestStopMovement);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoalListUpdated, FText, GoalList);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API UGOAPAgentComponent : public UActorComponent, public IAgent
{
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
	virtual FAIBlackboard& GetBlackboard() const override { return *Blackboard; }
	
	virtual const FWorldState& GetWorldState() const override { return WorldState; }
	virtual const TArray<const UAbstractAction*>& GetActions(EWorldPropertyKey Key) const override { return ActionList[static_cast<uint32>(Key)]; }

	virtual bool HasMood(const uint32 MoodFlag) const override;
	
	virtual void SetSprinting(const bool bEnable) override;
	virtual bool IsNear(const ENodeType NodeType) const override;

	virtual FVector GetFeetPosition() const override;

	virtual bool Goto(const ENodeType NodeType) override;
	virtual void Stop() override;
	virtual bool HasMovingFailed() const override;
	virtual bool HasFinishedMoving() const override;
	
	virtual bool CanPickup(const UResourceDefinition* Resource, const int32 Amount = 1) const override;
	virtual int32 GetTotalAmountInInventory() const override;
	virtual int32 GetAmountInInventory(const UResourceDefinition* Resource) const override;
	virtual int32 FindResourceSlot(const UResourceDefinition* Resource) const override;
	virtual int32 GetTotalAmountInInventoryByCategory(const EResourceCategory ResourceCategory) const override;
	virtual int32 GetNumInventorySlots() const override;
	virtual const FResourceStack& GetInventorySlot(const int32 SlotIndex) const override;

	virtual bool Harvest(AResourceNode* ResourceNode) override;
	virtual void DepositAll(AStockpile* Stockpile) override;
	virtual void DepositByCategory(AStockpile* Stockpile, const EResourceCategory ResourceCategory) override;
	
	virtual bool UseSlot(const int32 SlotIndex) override;
	
	virtual bool StartInteraction(ANeedModifierInteraction* Interaction) override;
	virtual void StopInteraction(ANeedModifierInteraction* Interaction) override;

	virtual ENeedSeverity GetNeedSeverity(const ENeedType NeedType) const override;
	virtual float GetNeedValue(const ENeedType NeedType) const override;
	
	UPROPERTY(BlueprintAssignable)
	FGoalListUpdated OnGoalListUpdated;
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	friend class FAIDebugComponentVisualizer;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void InitializeGoalsAndActions();
	void ResetPlan(bool bSuccess);	
	
	void InitWorldState(ASettlerCharacter* SettlerCharacter);
	bool ExecutePlan(ASettlerCharacter* SettlerCharacter);
	void EvaluateGoals();
	const UAbstractGoal* ChooseTopGoal();
	void OnGoalFailed(const UAbstractGoal* Goal);

	void SetDirty();
	bool IsBusy() const;
	
	UFUNCTION()
	void OnEmotionalStateChanged();
	void EvaluateMood();
	EAgentMood DetermineMoodFromEmotions(const FEmotionSummary& S);
		
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float LocationThreshold = 200.0f;
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
	FAIBlackboard* Blackboard = nullptr;

	FPlanner Planner;
	EInternalState State;

	bool bWorldIsDirty;
	FDelegateHandle HandleGlobalInventoryChanged;
	FDelegateHandle HandleGlobalBuildingSiteChanged;
	FDelegateHandle HandleGlobalResourceNodeChanged;
	FDelegateHandle HandleNeedSeverityChanged;

	TWeakObjectPtr<const UAbstractGoal> CurrentGoal;
	FWorldState CurrentGoalState;
	
	TArray<FPlanStep> Plan;
	int32 PlanStep = -1;
	bool bActionActivated = false;
	FAIState AIState;

	bool bMoveRequestDone = false;
	bool bMoveRequestFailed = false;
	
	bool bMoodDirty = false;
	EAgentMood Mood = EAgentMood::Neutral;
	
	float BusyTimer = 0;
};
