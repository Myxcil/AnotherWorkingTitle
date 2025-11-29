#pragma once

#include "../AIConstants.h"
#include "Settlers/Needs.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
struct FWorldProperty
{
	EWorldPropertyType		Type;
	union
	{
		int64				Value;			
		bool				bValue;
		ENodeType			NodeType;
		ENeedType			NeedType;
		UObject*			Object;
		EWorldPropertyKey	WorldPropertyKey;
	};

	FString ToString() const;
};
