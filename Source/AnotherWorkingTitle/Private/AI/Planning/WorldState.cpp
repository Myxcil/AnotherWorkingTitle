// (c) 2024 by Crenetic GmbH Studios

#include "AI/Planning/WorldState.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FWorldState::FWorldState()
{
	Flags = 0;
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		Property[KeyIndex].Type = EWorldPropertyType::Unknown;
		Property[KeyIndex].Value = 0;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FWorldState::~FWorldState()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::InitAllProperties()
{
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		Set(static_cast<EWorldPropertyKey>(KeyIndex), EWorldPropertyType::Unknown, 0);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Set(const EWorldPropertyKey Key, int32 Value)
{
	const uint32 KeyIndex = static_cast<uint32>(Key);
	Flags |= 1 << KeyIndex;
	Property[KeyIndex].Type = EWorldPropertyType::Int;
	Property[KeyIndex].Value = Value;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Set(const EWorldPropertyKey Key, bool bValue)
{
	const uint32 KeyIndex = static_cast<uint32>(Key);
	Flags |= 1 << KeyIndex;
	Property[KeyIndex].Type = EWorldPropertyType::Bool;
	Property[KeyIndex].bValue = bValue;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Set(const EWorldPropertyKey Key, const ENodeType NodeType)
{
	const uint32 KeyIndex = static_cast<uint32>(Key);
	Flags |= 1 << KeyIndex;
	Property[KeyIndex].Type = EWorldPropertyType::Node;
	Property[KeyIndex].NodeType = NodeType;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Set(const EWorldPropertyKey Key, ENeedType NeedType)
{
	const uint32 KeyIndex = static_cast<uint32>(Key);
	Flags |= 1 << KeyIndex;
	Property[KeyIndex].Type = EWorldPropertyType::NeedType;
	Property[KeyIndex].NeedType = NeedType;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Set(const EWorldPropertyKey Key, EResourceCategory ResourceCategory)
{
	const uint32 KeyIndex = static_cast<uint32>(Key);
	Flags |= 1 << KeyIndex;
	Property[KeyIndex].Type = EWorldPropertyType::ResourceCategory;
	Property[KeyIndex].ResourceCategory = ResourceCategory;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Set(const EWorldPropertyKey Key, const EWorldPropertyKey WorldPropertyKey)
{
	const uint32 KeyIndex = static_cast<uint32>(Key);
	Flags |= 1 << KeyIndex;
	Property[KeyIndex].Type = EWorldPropertyType::WorldPropertyKey;
	Property[KeyIndex].WorldPropertyKey = WorldPropertyKey;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Set(const EWorldPropertyKey Key, const EWorldPropertyType Type, const int64 Value)
{
	const uint32 KeyIndex = static_cast<uint32>(Key);
	Flags |= 1 << KeyIndex;
	Property[KeyIndex].Type = Type;
	Property[KeyIndex].Value = Value;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Set(const EWorldPropertyKey Key, const FWorldProperty* WorldProperty)
{
	Set(Key, WorldProperty->Type, WorldProperty->Value);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Clear()
{
	Flags = 0;
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		Property[KeyIndex].Type = EWorldPropertyType::Unknown;
		Property[KeyIndex].Value = 0;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FWorldState::Clear(const EWorldPropertyKey Key)
{
	const uint32 KeyIndex = static_cast<uint32>(Key);
	Flags &= ~(1 << KeyIndex);
	Property[KeyIndex].Type = EWorldPropertyType::Unknown;
	Property[KeyIndex].Value = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool FWorldState::Matches(const FWorldState& WorldState, const uint32 MatchFlags) const
{
	const uint32 ThisFlags = Flags & MatchFlags;
	const uint32 OtherFlags = WorldState.Flags & MatchFlags;
	if (ThisFlags != OtherFlags)
		return false;

	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const uint32 Flag = AI_ENUM_TO_FLAG(KeyIndex);
		if ((ThisFlags & Flag) != 0)
		{
			if (Property[KeyIndex].Type != WorldState.Property[KeyIndex].Type ||
				Property[KeyIndex].Value != WorldState.Property[KeyIndex].Value)
			{
				return false;
			}
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FWorldState::CountDifferences(const FWorldState& WorldState) const
{
	int32 Count = 0;
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const uint32 Flag = AI_ENUM_TO_FLAG(KeyIndex);
		const bool bIsSet = (Flags & Flag) != 0;
		const bool bIsOtherSet = (WorldState.Flags & Flag) != 0;
		if (bIsSet && bIsOtherSet)
		{
			if (Property[KeyIndex].Type != WorldState.Property[KeyIndex].Type ||
				Property[KeyIndex].Value != WorldState.Property[KeyIndex].Value)
			{
				++Count;
			}
		}
		else if (bIsSet || bIsOtherSet)
		{
			++Count;
		}
	}
	return Count;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FWorldState::CountUnsatisfied(const FWorldState& WorldState) const
{
	int32 Count = 0;
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const uint32 Flag = AI_ENUM_TO_FLAG(KeyIndex);
		if ((Flags & Flag) != 0)
		{
			if ((WorldState.Flags & Flag) != 0)
			{
				if (Property[KeyIndex].Type != WorldState.Property[KeyIndex].Type ||
					Property[KeyIndex].Value != WorldState.Property[KeyIndex].Value)
				{
					++Count;
				}
			}
			else
			{
				++Count;
			}
		}
	}
	return Count;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString FWorldState::ToString() const
{
	FString Result;
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);
		if (IsValid(Key))
		{
			Result.Append(GetWorldPropertyKeyName(Key));
			Result.AppendChar(':');
			Result.Append(Property[KeyIndex].ToString());
			Result.AppendChar('\n');
		}
	}
	return Result;
}

void FWorldState::WriteToLog() const
{
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);
		if (IsValid(Key))
		{
			AI_LOG(TEXT(" %s: %s"), *GetWorldPropertyKeyName(Key), *Property[KeyIndex].ToString())
		}
	}
}
