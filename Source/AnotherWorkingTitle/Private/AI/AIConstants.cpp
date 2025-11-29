// (c) 2024 by Crenetic GmbH Studios

#include "AI/AIConstants.h"

DEFINE_LOG_CATEGORY(LogGOAP);

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString GetWorldPropertyKeyName(EWorldPropertyKey Key)
{
	return StaticEnum<EWorldPropertyKey>()->GetNameStringByValue(static_cast<int64>(Key));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString GetWorldPropertyTypeName(EWorldPropertyType Type)
{
	return StaticEnum<EWorldPropertyType>()->GetNameStringByValue(static_cast<int64>(Type));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString GetFactMaskName(EFactMask FactMask)
{
	return StaticEnum<EFactMask>()->GetNameStringByValue(static_cast<int64>(FactMask));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FString GetBlackboardMaskName(EBlackboardMask BlackboardMask)
{
	return StaticEnum<EBlackboardMask>()->GetNameStringByValue(static_cast<int64>(BlackboardMask));
}
