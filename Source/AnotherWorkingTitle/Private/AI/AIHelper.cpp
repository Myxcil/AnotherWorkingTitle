// (c) 2024 by Crenetic GmbH Studios

//------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "AI/AIHelper.h"

#include "AI/AIBlackboard.h"
#include "AI/AIConstants.h"
#include "AI/IAgent.h"
#include "Construction/BuildingSite.h"
#include "Interactive/NeedsModifier.h"
#include "Inventory/Stockpile.h"
#include "Resources/ResourceNode.h"
#include "Resources/ResourceRegistrySubsystem.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FAIHelper::HasValidTransform(const IAgent& Agent, const ENodeType NodeType)
{
	switch (NodeType)
	{
	case ENodeType::ResourceNode:
	case ENodeType::Stockpile:
	case ENodeType::BuildingSite:
	case ENodeType::NeedsModifier:
		return true;
		
	default: 
		break;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FAIHelper::GetObjectTransform(const IAgent& Agent, const ENodeType NodeType, FTransform& Result)
{
	switch (NodeType)
	{
	case ENodeType::ResourceNode:
		if (const AResourceNode* ResourceNode = Agent.GetBlackboard().GetResourceNode())
		{
			Result = ResourceNode->GetActorTransform();
			return true;
		}
		break;
		
	case ENodeType::Stockpile:
		if (const AStockpile* Stockpile = Agent.GetBlackboard().GetStockpile())
		{
			Result = Stockpile->GetActorTransform();
			return true;
		}
		break;
		
	case ENodeType::BuildingSite:
		if (const ABuildingSite* BuildingSite = Agent.GetBlackboard().GetBuildingSite())
		{
			Result = BuildingSite->GetActorTransform();
			return true;
		}
		break;
		
	case ENodeType::NeedsModifier:
		if (const ANeedsModifier* NeedsModifier = Agent.GetBlackboard().GetNeedsModifier())
		{
			Result = NeedsModifier->GetActorTransform();
			return true;
		}
		break;
		
	default: 
		break;
	}
	
	AI_WARN(TEXT("GetObjectTransform, unhandled object"));
	Result = FTransform::Identity;
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const UResourceDefinition* FAIHelper::FindMinResourceInStockpiles(const UResourceRegistrySubsystem* ResourceRegistry)
{
	const TArray<const UResourceDefinition*>& AllResources = ResourceRegistry->GetAllResources();
	if (AllResources.Num() == 0)
		return nullptr;

	const TArray<AStockpile*>& Stockpiles = AStockpile::GetInstances();
	if (Stockpiles.Num() == 0)
		return nullptr;
	
	const UResourceDefinition* ScarcestResource = nullptr;
	int32 MinTotalAmount = TNumericLimits<int32>::Max();

	for (const UResourceDefinition* Resource : AllResources)
	{
		if (Resource == nullptr)
			continue;

		int32 TotalAmount = 0;

		for (const AStockpile* Stockpile : Stockpiles)
		{
			if (Stockpile == nullptr)
			{
				continue;
			}

			const FSettlementStock& Stock = Stockpile->GetSettlementStock();
			TotalAmount += Stock.GetAmount(Resource);
		}

		if (TotalAmount < MinTotalAmount)
		{
			MinTotalAmount   = TotalAmount;
			ScarcestResource = Resource;
			
			if (MinTotalAmount == 0)
				break;
		}
	}

	return ScarcestResource;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
AResourceNode* FAIHelper::FindNearestResourceNode(const FVector& RefPosition, const UResourceDefinition* Resource)
{
	if (!Resource)
		return nullptr;
	
	const TArray<AResourceNode*>& AllResourceNodes = AResourceNode::GetInstances();
	if (AllResourceNodes.Num() == 0)
		return nullptr;
	
	AResourceNode* NearestNode = nullptr;
	float SqMinDist = std::numeric_limits<float>::max();
	
	for (AResourceNode* ResourceNode : AllResourceNodes)
	{
		if (!ResourceNode)
			continue;
		
		if (!ResourceNode->HasResource(Resource))
			continue;
		
		const float SqDist = FVector::DistSquared(ResourceNode->GetActorLocation(), RefPosition);
		if (SqDist < SqMinDist)
		{
			SqMinDist = SqDist;
			NearestNode = ResourceNode;
		}
	}
	
	return NearestNode;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FAIHelper::HasResourceNodeByNeed(const ENeedType NeedType)
{
	const TArray<AResourceNode*>& AllResourceNodes = AResourceNode::GetInstances();
	for (AResourceNode* ResourceNode : AllResourceNodes)
	{
		if (!ResourceNode)
			continue;
		
		if (!ResourceNode->CanHarvest())
			continue;
		
		const UResourceDefinition* Resource = ResourceNode->GetResource();
		check(Resource);
		
		if (Resource->GetNeedChange(NeedType) < 0)
			return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
AResourceNode* FAIHelper::FindNearestResourceNodeByNeed(const FVector& RefPosition, const ENeedType NeedType)
{
	const TArray<AResourceNode*>& AllResourceNodes = AResourceNode::GetInstances();
	if (AllResourceNodes.Num() == 0)
		return nullptr;
	
	AResourceNode* NearestNode = nullptr;
	float SqMinDist = std::numeric_limits<float>::max();
	
	for (AResourceNode* ResourceNode : AllResourceNodes)
	{
		if (!ResourceNode)
			continue;
		
		const UResourceDefinition* Resource = ResourceNode->GetResource();
		check(Resource);
		
		if (Resource->GetNeedChange(NeedType) >= 0)
			continue;
		
		const float SqDist = FVector::DistSquared(ResourceNode->GetActorLocation(), RefPosition);
		if (SqDist < SqMinDist)
		{
			SqMinDist = SqDist;
			NearestNode = ResourceNode;
		}
	}
	
	return NearestNode;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
AStockpile* FAIHelper::FindNearestStockpile(const FVector& RefPosition)
{
	AStockpile* NearestPile = nullptr;
	float SqMinDist = std::numeric_limits<float>::max();
	
	for (AStockpile* Stockpile : AStockpile::GetInstances())
	{
		if (!Stockpile)
			continue;
		
		const float SqDist = FVector::DistSquared(Stockpile->GetActorLocation(), RefPosition);
		if (SqDist < SqMinDist)
		{
			SqMinDist = SqDist;
			NearestPile = Stockpile;
		}
	}
	
	return NearestPile;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float FAIHelper::CalculateResourceScarcity(const UResourceDefinition* Resource)
{
	constexpr int32 TestingHardLimit = 100;
	
	TMap<const UResourceDefinition*, int32> ResourceMap;
	for (AStockpile* Stockpile : AStockpile::GetInstances())
	{
		Stockpile->GetSettlementStock().Collect(ResourceMap);
	}
	if (!ResourceMap.Contains(Resource))
		return 1.0f;
	
	int32 TotalAmount = 0;
	for (const TTuple<const UResourceDefinition*, int>& Iter : ResourceMap)
	{
		TotalAmount += Iter.Value;
	}
	if (TotalAmount == 0)
		return 1.0f;
	
	if (TotalAmount >= TestingHardLimit)
		return 0.0f;
	
	const float RcpScale = 1.0f / TotalAmount;
	const float Scarcity = 1.0f - Clamp01(RcpScale * ResourceMap[Resource]);
	return 0.1f + 0.9f * Scarcity;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float FAIHelper::CalculateDistanceWeight(const FVector& From, const FVector& To, const float Scale)
{
	const float Distance = FVector::Distance(From, To);
	const float Weight = Scale * Distance;
	return FMath::Max(1.0f - Clamp01(Weight), Scale);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ABuildingSite* FAIHelper::FindNearestUnfinishedBuilding(const FVector& RefPosition)
{
	ABuildingSite* NearestSite = nullptr;
	float SqMinDist = std::numeric_limits<float>::max();
	
	const TArray<ABuildingSite*>& AlLBuildingSites = ABuildingSite::GetInstances();
	for (ABuildingSite* BuildingSite : AlLBuildingSites)
	{
		if (!BuildingSite)
			continue;
		
		if (!BuildingSite->CanConstruct())
			continue;
		
		const float SqDist = FVector::DistSquared(RefPosition, BuildingSite->GetActorLocation());
		if (SqDist < SqMinDist)
		{
			SqMinDist = SqDist;
			NearestSite = BuildingSite;
		}
	}
	
	return NearestSite;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ANeedsModifier* FAIHelper::FindNeedImprover(const ENeedType NeedType)
{
	for (ANeedsModifier* NeedsModifier : ANeedsModifier::GetInstances())
	{
		if (!NeedsModifier)
			continue;
		
		if (NeedsModifier->GetAffectedType() != NeedType)
			continue;
		
		if (NeedsModifier->GetNeedDelta() >= 0)
			continue;;
		
		return NeedsModifier;
	}
	return nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FAIHelper::FindFirstInInventoryByNeedChange(const IAgent& Agent, const ENeedType NeedType)
{
	for (int32 SlotIndex = 0; SlotIndex < Agent.GetNumInventorySlots(); ++SlotIndex)
	{
		const FResourceStack& Stack = Agent.GetInventorySlot(SlotIndex);
		if (Stack.IsValid())
		{
			const float NeedChange = Stack.Resource->GetNeedChange(NeedType);
			if (NeedChange < 0)
			{
				return SlotIndex;
			}
		}
	}
	return INDEX_NONE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FAIHelper::FindBestInInventoryByNeedChange(const IAgent& Agent, const ENeedType NeedType)
{
	int32 BestSlot = INDEX_NONE;
	float BestChange = std::numeric_limits<float>::max();
	for (int32 SlotIndex = 0; SlotIndex < Agent.GetNumInventorySlots(); ++SlotIndex)
	{
		const FResourceStack& Stack = Agent.GetInventorySlot(SlotIndex);
		if (Stack.IsValid())
		{
			const float NeedChange = Stack.Resource->GetNeedChange(NeedType);
			if (NeedChange != 0 && NeedChange < BestChange)
			{
				BestSlot = SlotIndex;
				BestChange = NeedChange;
			}
		}
	}
	return BestSlot;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FAIHelper::FindFirstInInventoryByCategory(const IAgent& Agent, const EResourceCategory ResourceCategory)
{
	for (int32 SlotIndex = 0; SlotIndex < Agent.GetNumInventorySlots(); ++SlotIndex)
	{
		const FResourceStack& Stack = Agent.GetInventorySlot(SlotIndex);
		if (Stack.IsValid())
		{
			const float NeedChange = Stack.Resource->Category == ResourceCategory;
			if (NeedChange < 0)
			{
				return SlotIndex;
			}
		}
	}
	return INDEX_NONE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 FAIHelper::FindFirstInInventoryByRuntimeId(const IAgent& Agent, const int32 ResourceId)
{
	for (int32 SlotIndex = 0; SlotIndex < Agent.GetNumInventorySlots(); ++SlotIndex)
	{
		const FResourceStack& Stack = Agent.GetInventorySlot(SlotIndex);
		if (Stack.IsValid())
		{
			const float NeedChange = Stack.Resource->RuntimeId == ResourceId;
			if (NeedChange < 0)
			{
				return SlotIndex;
			}
		}
	}
	return INDEX_NONE;
}
