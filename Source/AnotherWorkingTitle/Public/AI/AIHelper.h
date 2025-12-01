// (c) 2024 by Crenetic GmbH Studios

#pragma once

#include "CoreMinimal.h"
#include "AIConstants.h"
#include "Resources/ResourceCategory.h"
#include "Settlers/Needs.h"

class ANeedModifierInteraction;
class ANeedModifierArea;
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
	
	// World
	static bool HasValidTransform(const IAgent& Agent, const ENodeType NodeType, const bool bCheckBlackboardMembers);
	static bool GetObjectTransform(const IAgent& Agent, const ENodeType NodeType, FTransform& Result);
	static float CalculateDistanceWeight(const FVector& From, const FVector& To, const float Scale = 0.0001f);
	
	// Resources
	static bool HasResourceNode(const UResourceDefinition* Resource);
	static AResourceNode* FindNearestResourceNode(const FVector& RefPosition, const UResourceDefinition* Resource);
	static bool HasResourceNodeByNeed(const ENeedType NeedType);
	static AResourceNode* FindNearestResourceNodeByNeed(const FVector& RefPosition, const ENeedType NeedType);
	
	static float CalculateResourceScarcity(const UResourceDefinition* Resource);

	// Storage
	static AStockpile* FindNearestStockpile(const FVector& RefPosition);
	
	// Construction
	static ABuildingSite* FindNearestUnfinishedBuilding(const FVector& RefPosition);
	
	// Needs
	static bool HasNeedImprover(const ENeedType NeedType);
	static ANeedModifierArea* FindNearestNeedImprover(const FVector& RefPosition, const ENeedType NeedType);
	static bool HasNeedInteraction(const ENeedType NeedType);
	static ANeedModifierInteraction* FindNearestNeedInteraction(const FVector& RefPosition, const ENeedType NeedType);
	
	// Inventory
	static int32 FindFirstInInventoryByNeedChange(const IAgent& Agent, const ENeedType NeedType);
	static int32 FindBestInInventoryByNeedChange(const IAgent& Agent, const ENeedType NeedType);
	
	static int32 FindFirstInInventoryByCategory(const IAgent& Agent, const EResourceCategory ResourceCategory);
	static int32 FindFirstInInventoryByRuntimeId(const IAgent& Agent, const int32 ResourceId);
};
