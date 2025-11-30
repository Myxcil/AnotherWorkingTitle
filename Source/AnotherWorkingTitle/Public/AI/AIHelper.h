// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "AIConstants.h"
#include "Resources/ResourceCategory.h"
#include "Settlers/Needs.h"

class IAgent;
class ABuildingSite;
class AStockpile;
class AResourceNode;
class UResourceRegistrySubsystem;
class UResourceDefinition;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class ANOTHERWORKINGTITLE_API FAIHelper
{
public:
	FORCEINLINE static float Clamp01(const float InValue) { return FMath::Clamp(InValue, 0.0f, 1.0f); }
	
	static bool HasValidTransform(const IAgent& Agent, const ENodeType NodeType);
	static bool GetObjectTransform(const IAgent& Agent, const ENodeType NodeType, FTransform& Result);
	
	static const UResourceDefinition* FindMinResourceInStockpiles(const UResourceRegistrySubsystem* ResourceRegistry);
	static AResourceNode* FindNearestResourceNode(const FVector& RefPosition, const UResourceDefinition* Resource);
	static AResourceNode* FindNearestResourceNodeByNeed(const FVector& RefPosition, const ENeedType NeedType);
	
	static AStockpile* FindNearestStockpile(const FVector& RefPosition);
	
	static float CalculateResourceScarcity(const UResourceDefinition* Resource);

	static float CalculateDistanceWeight(const FVector& From, const FVector& To, const float Scale = 0.0001f);
	
	static ABuildingSite* FindNearestUnfinishedBuilding(const FVector& RefPosition);
	
	static int32 FindFirstInInventoryByNeedChange(const IAgent& Agent, const ENeedType NeedType);
	static int32 FindBestInInventoryByNeedChange(const IAgent& Agent, const ENeedType NeedType);
	
	static int32 FindFirstInInventoryByCategory(const IAgent& Agent, const EResourceCategory ResourceCategory);
	static int32 FindFirstInInventoryByRuntimeId(const IAgent& Agent, const int32 ResourceId);
};
