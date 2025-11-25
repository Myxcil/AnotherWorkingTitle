// (c) 2025 MK


#include "AI/GOAPAgentComponent.h"

#include "AI/AIHelper.h"
#include "AI/AISettings.h"
#include "AI/Actions/AbstractAction.h"
#include "AI/Goals/AbstractGoal.h"
#include "GameFramework/CharacterMovementComponent.h"
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
void UGOAPAgentComponent::TickGOAP(const float DeltaSeconds)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::OnMovementComplete(bool bSuccess)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::OnActionComplete(bool bSuccess)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGOAPAgentComponent::OnWorldStateChange()
{
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
	
	return FVector::Distance(ObjecTransform.GetLocation(), GetGroundPosition()) <= SettingsPtr->NearRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FVector UGOAPAgentComponent::GetGroundPosition() const
{
	if (const ASettlerCharacter* Settler = SettlerPtr.Get())
	{
		return Settler->GetCharacterMovement()->GetActorFeetLocation();
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
			return OnAIRequestMoveToLocation.Execute(Transform.GetLocation(), DefaultAcceptanceRadius);
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
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGOAPAgentComponent::HasFinishedMoving() const
{
	return false;
}
