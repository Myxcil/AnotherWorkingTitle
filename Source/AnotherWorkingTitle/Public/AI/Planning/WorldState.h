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
	void Set(const EWorldPropertyKey Key, EWorldPropertyKey WorldPropertyKey);
	void Set(const EWorldPropertyKey Key, UObject* Object);

	void Set(const EWorldPropertyKey Key, const FWorldProperty* WorldProperty);

	const FWorldProperty* Get(const EWorldPropertyKey Key) const { return IsValid(Key) ? Property + static_cast<uint32>(Key) : nullptr; }

	template<typename T>
	T* GetObject(const EWorldPropertyKey Key) const
	{
		const FWorldProperty* WorldProperty = Get(Key);
		return (WorldProperty && WorldProperty->Type == EWorldPropertyType::Object) ? Cast<T>(WorldProperty->Object) : nullptr; 
	}
		
	void Clear();
	void Clear(const EWorldPropertyKey Key);

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	bool IsSet() const { return Flags != 0; }
	bool IsValid(EWorldPropertyKey Key) const { return (Flags & WORLD_PROPERTY_KEY_FLAG(Key)) != 0; }

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
