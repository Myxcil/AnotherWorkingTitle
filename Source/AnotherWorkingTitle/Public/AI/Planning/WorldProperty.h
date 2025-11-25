#pragma once

#include "../AIConstants.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
struct FWorldProperty
{
	EWorldPropertyType		Type;
	union
	{
		int64				Value;			
		bool				bValue;
		ENodeType			NodeType;
		UObject*			Object;
		EWorldPropertyKey	WorldPropertyKey;
	};

	FString ToString() const;
};
