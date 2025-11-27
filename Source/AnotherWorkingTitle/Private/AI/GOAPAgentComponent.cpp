// (c) 2025 MK


#include "AI/GOAPAgentComponent.h"

#include "AI/AIHelper.h"
#include "AI/AISettings.h"
#include "AI/Actions/AbstractAction.h"
#include "AI/Goals/AbstractGoal.h"
#include "Inventory/InventoryComponent.h"
#include "Resources/ResourceNode.h"
#include "Resources/ResourceRegistrySubsystem.h"
#include "Settlers/NeedsComponent.h"
#include "Settlers/SettlerCharacter.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UGOAPAgentComponent::UGOAPAgentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SettingsPtr = GetDefault<UAISettings>();
	SettlerPtr = Cast<ASettlerCharacter>(GetOwner());
	ResourceRegistry = GetWorld()->GetGameInstance()->GetSubsystem<UResourceRegistrySubsystem>();
	
	ResetPlan(false);
	State = EInternalState::Idle;
	bWorldIsDirty = true;
	
	HandleGlobalInventoryChanged = FInventoryBase::OnInventoryChanged.AddUObject(this, &ThisClass::SetDirty);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HandleGlobalInventoryChanged.IsValid())
	{
		FInventoryBase::OnInventoryChanged.Remove(HandleGlobalInventoryChanged);
		HandleGlobalInventoryChanged.Reset();
	}
	
	Super::EndPlay(EndPlayReason);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::InitializeAgent()
{
	InitializeGoalsAndActions();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::InitializeGoalsAndActions()
{
	static TArray<UClass*> DefaultGoalClasses;
	if (DefaultGoalClasses.IsEmpty())
	{
		GetDerivedClasses(UAbstractGoal::StaticClass(), DefaultGoalClasses, true);
		DefaultGoalClasses.RemoveAll([&](const UClass* Class)
		{
			return Class->HasAnyClassFlags(CLASS_Abstract);
		});
	}
	for (int32 I=0; I < DefaultGoalClasses.Num(); ++I)
	{
		GoalList.AddUnique(FGoalInstance(DefaultGoalClasses[I]->GetDefaultObject<UAbstractGoal>()));
	}
	
	static TArray<UClass*> DefaultActionClasses;
	if (DefaultActionClasses.IsEmpty())
	{
		GetDerivedClasses(UAbstractAction::StaticClass(), DefaultActionClasses, true);
		DefaultActionClasses.RemoveAll([&](const UClass* Class)
		{
			return Class->HasAnyClassFlags(CLASS_Abstract);
		});
	}

	TArray<const UAbstractAction*> Actions;
	for(int32 I=0; I < DefaultActionClasses.Num(); ++I)
	{
		Actions.AddUnique(DefaultActionClasses[I]->GetDefaultObject<UAbstractAction>());
	}

	for (int32 KeyIndex = 0; KeyIndex < ActionList.Num(); ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);
		for (int32 I = 0; I < Actions.Num(); ++I)
		{
			if (Actions[I]->HasResult(Key))
			{
				ActionList[KeyIndex].Add(Actions[I]);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::ResetPlan(bool bSuccess)
{
	Stop();
	
	if (bActionActivated && Plan.IsValidIndex(PlanStep))
	{
		Plan[PlanStep].Action->Deactivate(*this, AIState);
	}

	Plan.Empty();
	PlanStep = -1;
	bActionActivated = false;

	if (CurrentGoal != nullptr)
	{
		if (bSuccess)
		{
			AI_LOG(TEXT("%s finished %s"), *SettlerPtr->GetName(), *CurrentGoal->GetTypeName());
		}
		else
		{
			AI_WARN(TEXT("%s interrupt %s"), *SettlerPtr->GetName(), *CurrentGoal->GetTypeName());
		}

		CurrentGoal->DeInit(*this, bSuccess);
		CurrentGoal = nullptr;
	}

	CurrentGoalState.Clear();

	bMoveRequestDone = false;
	bMoveRequestFailed = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::TickGOAP(const float DeltaSeconds)
{
	ASettlerCharacter* SettlerCharacter = SettlerPtr.Get();
	if (!SettlerCharacter)
		return;
	
	if (BusyTimer > 0)
	{
		BusyTimer -= DeltaSeconds;
	}
	
	if (IsBusy())
		return;
	
	if (Memory.IsModified())
	{
		bWorldIsDirty = true;
	}
	
	if (State == EInternalState::Paused)
		return;
	
	if (State == EInternalState::IdleFailed && bWorldIsDirty)
	{
		State = EInternalState::Idle;
	}

	switch (State)
	{
	case EInternalState::Idle:
	case EInternalState::IdleFailed:
		break;
		
	case EInternalState::Planning:
		{
			InitWorldState(SettlerCharacter);
		
			// A new goal has been selected, create a plan (if possible)
			// and switch to the next internal state of executing it or return to idle
			const UAbstractGoal* Goal = CurrentGoal.Get();
			
			FWorldState GoalState;
			float Cost;
			if (Planner.FindValidPlan(*this, Goal, GoalState, Cost))
			{
				State = EInternalState::Executing;
			}
			else
			{
				OnGoalFailed(Goal);
			}
		}
		break;
		
	case EInternalState::Executing:
		{
			if (!ExecutePlan(SettlerCharacter))
			{
				OnGoalFailed(CurrentGoal.Get());
			}
		}
		break;
		
	case EInternalState::AwaitApproval:
		break;
		
	case EInternalState::Paused:
		break;
	}
	
	EvaluateGoals();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::InitWorldState(ASettlerCharacter* SettlerCharacter)
{
	WorldState.Clear();

	for(int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey PropertyKey = static_cast<EWorldPropertyKey>(KeyIndex);
		switch (PropertyKey)
		{
		case EWorldPropertyKey::Harvest:
			WorldState.Set(PropertyKey, static_cast<UObject*>(nullptr));
			break;
			
		default:
			WorldState.Set(PropertyKey, 0);
			break;
		}
	}
	
	SetSprinting(false);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::ExecutePlan(ASettlerCharacter* SettlerCharacter)
{
	// Plan just started, init goal
	if (PlanStep == -1)
	{
		if (CurrentGoal->Evaluate(*this) <= 0)
		{
			AI_LOG(TEXT("%s %s became invalid"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName());
			CurrentGoal = nullptr;
			return false;
		}
		
		AI_LOG(TEXT("%s executes %s"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName());

		// initialize goal but this time for real 
		if (!CurrentGoal->Init(*this, CurrentGoalState, false))
		{
			AI_LOG(TEXT("%s failed to init %s"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName());
			return false;
		}

		if (!Planner.GeneratePlan(*this, CurrentGoal.Get(), CurrentGoalState, Plan))
		{
			AI_LOG(TEXT("%s failed to generate plan for %s"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName());
			return false;
		}
		
		for (int32 I=0; I < Plan.Num(); ++I)
		{
			Plan[I].WriteToLog();
		}
		
		PlanStep = 0;
		bActionActivated = false;
	}
	// in the middle of the plan, handle actions
	else if (PlanStep < Plan.Num())
	{
		check(Plan.IsValidIndex(PlanStep));
		check(CurrentGoal.IsValid());
		const UAbstractAction* Action = Plan[PlanStep].Action;
		const FWorldState& CurrentWorldState = Plan[PlanStep].WorldState;

		if (!bActionActivated)
		{
			AI_LOG(TEXT("%s %s: execute %s"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName(), *Action->GetTypeName())

			if (!Action->AreContextPreconditionsSatisfied(*this, CurrentWorldState, false))
			{
				AI_WARN(TEXT("%s %s: failed context preconditions %s"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName(), *Action->GetTypeName())
				return false;
			}

			if (!Action->Activate(*this, AIState, CurrentWorldState))
			{
				AI_WARN(TEXT("%s %s: failed activation %s"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName(), *Action->GetTypeName())
				return false;
			}

			bActionActivated = true;
		}

		const EActionResult Result = Action->IsComplete(*this, AIState);
		if (Result == EActionResult::Failed)
		{
			AI_WARN(TEXT("%s %s: failed %s"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName(), *Action->GetTypeName())
			return false;
		}

		if (Result == EActionResult::Complete)
		{
			AI_LOG(TEXT("%s %s: finished %s"), *SettlerCharacter->GetName(), *CurrentGoal->GetTypeName(), *Action->GetTypeName())
			Action->Deactivate(*this, AIState);
			Action->ApplyResults(*this, WorldState, CurrentWorldState);
			bActionActivated = false;
			if (++PlanStep == Plan.Num())
			{
				ResetPlan(true);
				State = EInternalState::Idle;
			}
		}
	}
	else
	{
		AI_WARN(TEXT("%s Shouldn't happen, bad plan index"), *SettlerCharacter->GetName());
		ResetPlan(false);
		State = EInternalState::Idle;
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::EvaluateGoals()
{
	if (State != EInternalState::IdleFailed)
	{
		// goal change occurred? Reset current goal and plan
		const UAbstractGoal* NextGoal = ChooseTopGoal();
		if (NextGoal != CurrentGoal)
		{
			ResetPlan(false);
			CurrentGoal = NextGoal;
			if (CurrentGoal != nullptr)
			{
				State = EInternalState::Planning;
			}
			else
			{
				State = EInternalState::IdleFailed;
			}
		}
		else if (NextGoal == nullptr)
		{
			State = EInternalState::IdleFailed;
			CurrentGoal = nullptr;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const UAbstractGoal* UGOAPAgentComponent::ChooseTopGoal()
{
	const int32 NumGoals = GoalList.Num();
	if (NumGoals == 0)
		return nullptr;
	
	const UAbstractGoal* NextGoal = CurrentGoal.Get();

	// if world view has changed, re-evaluate all goals
	// store best rated goal	
	if (bWorldIsDirty)
	{
		for (int32 i = 0; i < NumGoals; ++i)
		{
			GoalList[i].Priority = GoalList[i].Goal->Evaluate(*this);
		}
	
		GoalList.Sort([](const FGoalInstance& A, const FGoalInstance& B)
		{
			return A.Priority > B.Priority; 
		});
	
		AI_LOG(TEXT("%s evaluated goals"), *SettlerPtr->GetName());
		for (int32 I=0; I < NumGoals; ++I)
		{
			AI_LOG(TEXT(" %.4f %s"), GoalList[I].Priority, *GoalList[I].Goal->GetTypeName())
		}
		
		bWorldIsDirty = false;
	}

	for (int32 I=0; I < NumGoals; ++I)
	{
		if (GoalList[I].Priority > 0)
		{
			NextGoal = GoalList[I].Goal;
			break;
		}
	}
	
	return NextGoal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::OnGoalFailed(const UAbstractGoal* Goal)
{
	const int32 NumGoals = GoalList.Num();
	for (int32 I=0; I < NumGoals; ++I)
	{
		if (GoalList[I].Goal == Goal)
		{
			GoalList[I].Priority = 0;
			break;
		}
	}
	
	ResetPlan(false);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::SetDirty()
{
	bWorldIsDirty = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::IsBusy() const
{
	if (BusyTimer > 0)
		return true;
	
	if (const ASettlerCharacter* Settler = SettlerPtr.Get())
	{
		if (Settler->IsBusy())
			return true;
	}
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::OnMovementComplete(bool bSuccess)
{
	bMoveRequestDone = true;
	bMoveRequestFailed = !bSuccess;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UResourceRegistrySubsystem* UGOAPAgentComponent::GetResourceRegistry() const
{
	return ResourceRegistry.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::SetSprinting(const bool bEnable)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::IsNear(const UObject* Object) const
{
	FTransform ObjecTransform;
	if (!FAIHelper::GetObjectTransform(Object, ObjecTransform))
		return false;
	
	return FVector::Distance(ObjecTransform.GetLocation(), GetFeetPosition()) <= SettingsPtr->NearRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FVector UGOAPAgentComponent::GetFeetPosition() const
{
	if (const ASettlerCharacter* Settler = SettlerPtr.Get())
	{
		return Settler->GetFeetPosition();
	}
	return FVector::ZeroVector;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::SearchNodePosition(const ENodeType Node)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::IsSearchDone() const
{
	return !bEQSRunning;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::Goto(UObject* Object)
{
	if (OnAIRequestMoveToLocation.IsBound())
	{
		FTransform Transform;
		if (FAIHelper::GetObjectTransform(Object, Transform))
		{
			bMoveRequestDone = false;
			bMoveRequestFailed = false;
			return OnAIRequestMoveToLocation.Execute(Transform.GetLocation(), LocationThreshold, DefaultAcceptanceRadius);
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::Stop()
{
	if (OnAIRequestStopMovement.IsBound())
	{
		OnAIRequestStopMovement.Execute();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::HasMovingFailed() const
{
	return bMoveRequestFailed;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::HasFinishedMoving() const
{
	return bMoveRequestDone;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::CanPickup(const UResourceDefinition* Resource, const int32 Amount) const
{
	if (Resource && Amount > 0)
	{
		if (const ASettlerCharacter* Settler = SettlerPtr.Get())
		{
			if (const UInventoryComponent* InventoryComponent = Settler->GetInventoryComponent())
			{
				const FInventory& Inventory = InventoryComponent->GetInventory();
				return Inventory.CanCarryMore(Resource, Amount);
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 UGOAPAgentComponent::GetTotalAmountInInventory() const
{
	if (const ASettlerCharacter* Settler = SettlerPtr.Get())
	{
		if (const UInventoryComponent* InventoryComponent = Settler->GetInventoryComponent())
		{
			const FInventory& Inventory = InventoryComponent->GetInventory();
			return Inventory.GetTotalAmount();
		}
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 UGOAPAgentComponent::GetAmountInInventory(const UResourceDefinition* Resource) const
{
	if (const ASettlerCharacter* Settler = SettlerPtr.Get())
	{
		if (const UInventoryComponent* InventoryComponent = Settler->GetInventoryComponent())
		{
			const FInventory& Inventory = InventoryComponent->GetInventory();
			return Inventory.GetAmount(Resource);
		}
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::Harvest(AResourceNode* ResourceNode)
{
	if (ResourceNode)
	{
		if (ASettlerCharacter* Settler = SettlerPtr.Get())
		{
			ResourceNode->Harvest(Settler);
			BusyTimer = 0.5f;
			SetDirty();
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::DepositAll(AStockpile* Stockpile)
{
	if (Stockpile)
	{
		if (ASettlerCharacter* Settler = SettlerPtr.Get())
		{
			Stockpile->TransferWholeInventory(Settler);
			BusyTimer = 0.5f;
			SetDirty();
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::IsInCriticalState() const
{
	if (const ASettlerCharacter* Settler = SettlerPtr.Get())
	{
		if (const UNeedsComponent* NeedsComponent = Settler->GetNeedsComponent())
		{
			return NeedsComponent->IsAnyNeedCritical();
		}
	}
	return false;
}
