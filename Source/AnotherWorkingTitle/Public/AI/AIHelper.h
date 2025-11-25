// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"

class AStockpile;
class AResourceNode;
class UResourceRegistrySubsystem;
class UResourceDefinition;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class ANOTHERWORKINGTITLE_API FAIHelper
{
public:
	static bool GetObjectTransform(const UObject* Object, FTransform& Result);
	
	static const UResourceDefinition* FindMinResourceInStockpiles(const UResourceRegistrySubsystem* ResourceRegistry);
	static AResourceNode* FindNearestResourceNode(const FVector& RefPosition, const UResourceDefinition* Resource);
	static AStockpile* FindNearestStockpile(const FVector& RefPosition);
};
