#pragma once

#include "../AIConstants.h"
#include "Resources/ResourceCategory.h"
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
		EResourceCategory	ResourceCategory;

		EWorldPropertyKey	WorldPropertyKey;
	};

	FString ToString() const;
};
