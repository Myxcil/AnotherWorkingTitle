#pragma once

#include "WorldProperty.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class FWorldState
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FWorldState();
	~FWorldState();

	void InitAllProperties();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void Set(const EWorldPropertyKey Key, int32 Value);
	void Set(const EWorldPropertyKey Key, bool bValue);
	void Set(const EWorldPropertyKey Key, ENodeType NodeType);
	void Set(const EWorldPropertyKey Key, ENeedType NeedType);
	void Set(const EWorldPropertyKey Key, EResourceCategory ResourceCategory);
	void Set(const EWorldPropertyKey Key, EWorldPropertyKey WorldPropertyKey);

	void Set(const EWorldPropertyKey Key, const FWorldProperty* WorldProperty);

	const FWorldProperty* Get(const EWorldPropertyKey Key) const { return IsValid(Key) ? Property + static_cast<uint32>(Key) : nullptr; }
		
	void Clear();
	void Clear(const EWorldPropertyKey Key);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	bool IsSet() const { return Flags != 0; }
	bool IsValid(EWorldPropertyKey Key) const { return (Flags & AI_ENUM_TO_FLAG(Key)) != 0; }

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	bool Matches(const FWorldState& WorldState, uint32 MatchFlags = 0xffffffff) const;
	int32 CountDifferences(const FWorldState& WorldState) const;
	int32 CountUnsatisfied(const FWorldState& WorldState) const;

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FString ToString() const;
	void WriteToLog() const;

private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void Set(const EWorldPropertyKey Key, EWorldPropertyType Type, int64 Value);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	uint32 Flags;
	FWorldProperty Property[WorldPropertyKeyCount];
};
