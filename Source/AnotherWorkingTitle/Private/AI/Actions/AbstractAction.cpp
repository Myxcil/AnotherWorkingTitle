// (c) 2024 by Crenetic GmbH Studios

#include "AI/Actions/AbstractAction.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
FString UAbstractAction::GetTypeName() const
{
	const UClass* ActionClass = GetClass();
	check(ActionClass);
	const FName ClassName = ActionClass->GetFName();
	const FString ClassString = ClassName.ToString();
	return ClassString.RightChop(6);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UAbstractAction::CanSolveWorldState(const FWorldState& CurrentWorldState, const FWorldState& GoalWorldState) const
{
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);

		const FWorldProperty* Result = Results.Get(Key);
		if (Result == nullptr)
			continue;

		const FWorldProperty* WorldPropertyCurrent = CurrentWorldState.Get(Key);
		if (Result->Type != EWorldPropertyType::WorldPropertyKey)
		{
			if (WorldPropertyCurrent != nullptr && 
				WorldPropertyCurrent->Type == Result->Type &&
				WorldPropertyCurrent->Value == Result->Value)
			{
				continue;
			}
		}
		else
		{
			const FWorldProperty* WorldPropertyGoal = GoalWorldState.Get(Result->WorldPropertyKey);
			if (WorldPropertyCurrent != nullptr && WorldPropertyGoal != nullptr &&
				WorldPropertyCurrent->Type == WorldPropertyGoal->Type &&
				WorldPropertyCurrent->Value == WorldPropertyGoal->Value)
			{
				continue;
			}
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractAction::ApplyPreconditions(IAgent& Agent, FWorldState& GoalState) const
{
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);

		const FWorldProperty* WorldPropertyPrecondition = Preconditions.Get(Key);
		if (WorldPropertyPrecondition == nullptr)
			continue;

		if (WorldPropertyPrecondition->Type != EWorldPropertyType::WorldPropertyKey)
		{
			GoalState.Set(Key, WorldPropertyPrecondition);
		}
		else
		{
			const FWorldProperty* WorldPropertyGoal = GoalState.Get(WorldPropertyPrecondition->WorldPropertyKey);
			if (WorldPropertyGoal != nullptr)
			{
				GoalState.Set(Key, WorldPropertyGoal);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractAction::ApplyResults(IAgent& Agent, FWorldState& CurrentWorldState, const FWorldState& GoalWorldState) const
{
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);

		const FWorldProperty* WorldPropertyResult = Results.Get(Key);
		if (WorldPropertyResult == nullptr)
			continue;

		if (WorldPropertyResult->Type != EWorldPropertyType::WorldPropertyKey)
		{
			CurrentWorldState.Set(Key, WorldPropertyResult);
		}
		else
		{
			const FWorldProperty* WorldPropertyGoal = GoalWorldState.Get(WorldPropertyResult->WorldPropertyKey);
			if (WorldPropertyGoal != nullptr)
			{
				CurrentWorldState.Set(Key, WorldPropertyGoal);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractAction::SolveWorldState(FWorldState& CurrentWorldState, const FWorldState& GoalWorldState) const
{
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);

		const FWorldProperty* WorldPropertyResult = Results.Get(Key);
		if (WorldPropertyResult == nullptr)
			continue;

		const FWorldProperty* WorldProperty;
		if (WorldPropertyResult->Type != EWorldPropertyType::WorldPropertyKey)
		{
			WorldProperty = GoalWorldState.Get(Key);
		}
		else
		{
			WorldProperty = GoalWorldState.Get(WorldPropertyResult->WorldPropertyKey);
		}

		if (WorldProperty != nullptr)
		{
			CurrentWorldState.Set(Key, WorldProperty);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UAbstractAction::ArePreconditionsSatisfied(const FWorldState& CurrentWorldState, const FWorldState& GoalState, EWorldPropertyKey& FailedKey) const
{
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);

		const FWorldProperty* Precondition = Preconditions.Get(Key);
		if (Precondition == nullptr)
			continue;

		const FWorldProperty* WorldPropertyCurrent = CurrentWorldState.Get(Key);

		if (Precondition->Type != EWorldPropertyType::WorldPropertyKey)
		{
			if (WorldPropertyCurrent != nullptr)
			{
				if (WorldPropertyCurrent->Type != Precondition->Type ||
					WorldPropertyCurrent->Value != Precondition->Value)
				{
					if (&FailedKey != nullptr) FailedKey = Key;
					return false;
				}
			}
		}
		else
		{
			const FWorldProperty* WorldPropertyGoal = GoalState.Get(Precondition->WorldPropertyKey);
			if (WorldPropertyCurrent != nullptr && WorldPropertyGoal != nullptr)
			{
				if (WorldPropertyCurrent->Type != WorldPropertyGoal->Type ||
					WorldPropertyCurrent->Value != WorldPropertyGoal->Value)
				{
					if (&FailedKey != nullptr) FailedKey = Key;
					return false;
				}
			}
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UAbstractAction::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void UAbstractAction::Deactivate(IAgent& Agent) const
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UAbstractAction::IsComplete(IAgent& Agent) const
{
	return EActionResult::Incomplete;
}
