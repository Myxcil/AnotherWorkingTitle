// (c) 2024 by Crenetic GmbH Studios

#include "AI/Planning/WorldProperty.h"

template<typename E>
static FString GetEnumValueName(E EnumValue)
{
	const UEnum* Enum = StaticEnum<E>();
	const int32 EnumNameLength = Enum->GetName().Len();
	const FString FullValueName = Enum->GetNameStringByValue(static_cast<int64>(EnumValue));
	//return FullValueName.RightChop(EnumNameLength+2);
	return FullValueName;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString FWorldProperty::ToString() const
{
	switch (Type)
	{
	default:
		return TEXT("Unknown");
	case EWorldPropertyType::Int:
		return FString::FromInt(Value);
	case EWorldPropertyType::Bool:
		return bValue ? TEXT("true") : TEXT("false");
	case EWorldPropertyType::Node:
		return GetEnumValueName(NodeType);
	case EWorldPropertyType::NeedType:
		return GetEnumValueName(NeedType);
	case EWorldPropertyType::WorldPropertyKey:
		return GetWorldPropertyKeyName(WorldPropertyKey);
	}
}
