// (c) 2024 by Crenetic GmbH Studios

//------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "AI/AIHelper.h"

#include "AI/AIBlackboard.h"
#include "AI/AIConstants.h"
#include "AI/IAgent.h"
#include "Construction/BuildingSite.h"
#include "Interactive/NeedModifierInteraction.h"
#include "Interactive/NeedModifierArea.h"
#include "Inventory/Stockpile.h"
#include "Resources/ResourceNode.h"
#include "Resources/ResourceRegistrySubsystem.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FAIHelper::HasValidTransform(const IAgent& Agent, const ENodeType NodeType, const bool bCheckBlackboardMembers)
{
	const FAIBlackboard& Blackboard = Agent.GetBlackboard();
	
	switch (NodeType)
	{
	case ENodeType::ResourceNode:
		return !bCheckBlackboardMembers || Blackboard.IsSet(EBlackboardMask::ResourceNode);
	case ENodeType::Stockpile:
		return !bCheckBlackboardMembers || Blackboard.IsSet(EBlackboardMask::Stockpile);
	case ENodeType::BuildingSite:
		return !bCheckBlackboardMembers || Blackboard.IsSet(EBlackboardMask::BuildingSite);
	case ENodeType::NeedsModifier:
		return !bCheckBlackboardMembers || Blackboard.IsSet(EBlackboardMask::NeedsModifier);
	case ENodeType::NeedInteraction:
		return !bCheckBlackboardMembers || Blackboard.IsSet(EBlackboardMask::NeedInteraction);
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
		if (const ANeedModifierArea* NeedsModifier = Agent.GetBlackboard().GetNeedsModifier())
		{
			Result = NeedsModifier->GetActorTransform();
			return true;
		}
		break;
		
	case ENodeType::NeedInteraction:
		if (const ANeedModifierInteraction* NeedInteraction = Agent.GetBlackboard().GetNeedInteraction())
		{
			Result = NeedInteraction->GetActorTransform();
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
bool FAIHelper::HasResourceNode(const UResourceDefinition* Resource)
{
	if (!Resource)
		return false;

	for (AResourceNode* ResourceNode : AResourceNode::GetInstances())
	{
		if (!ResourceNode)
			continue;
		
		if (!ResourceNode->HasResource(Resource))
			continue;
		
		return true;
	}
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
AResourceNode* FAIHelper::FindNearestResourceNode(const FVector& RefPosition, const UResourceDefinition* Resource)
{
	if (!Resource)
		return nullptr;
	
	AResourceNode* NearestNode = nullptr;
	float SqMinDist = std::numeric_limits<float>::max();
	
	for (AResourceNode* ResourceNode : AResourceNode::GetInstances())
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
	
	for (ABuildingSite* BuildingSite : ABuildingSite::GetInstances())
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
bool FAIHelper::HasNeedImprover(const ENeedType NeedType)
{
	for (ANeedModifierArea* NeedsModifier : ANeedModifierArea::GetInstances())
	{
		if (!NeedsModifier)
			continue;
		
		if (NeedsModifier->GetAffectedType() != NeedType)
			continue;
		
		if (NeedsModifier->GetNeedDelta() >= 0)
			continue;;
		
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ANeedModifierArea* FAIHelper::FindNearestNeedImprover(const FVector& RefPosition, const ENeedType NeedType)
{
	ANeedModifierArea* NearestNeedsModifier = nullptr;
	float SqMinDist = std::numeric_limits<float>::max();
	
	for (ANeedModifierArea* NeedsModifier : ANeedModifierArea::GetInstances())
	{
		if (!NeedsModifier)
			continue;
		
		if (NeedsModifier->GetAffectedType() != NeedType)
			continue;
		
		if (NeedsModifier->GetNeedDelta() >= 0)
			continue;;
		
		const float SqDist = FVector::DistSquared(RefPosition, NeedsModifier->GetActorLocation());
		if (SqDist < SqMinDist)
		{
			SqMinDist = SqDist;
			NearestNeedsModifier = NeedsModifier;
		}
	}
	return NearestNeedsModifier;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FAIHelper::HasNeedInteraction(const ENeedType NeedType)
{
	for (ANeedModifierInteraction* NeedInteraction : ANeedModifierInteraction::GetInstances())
	{
		if (!NeedInteraction)
			continue;
		
		if (NeedInteraction->GetAffectedType() != NeedType)
			continue;
		
		if (NeedInteraction->GetNeedDelta() >= 0)
			continue;;
		
		if (NeedInteraction->IsInteracting())
			continue;
		
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ANeedModifierInteraction* FAIHelper::FindNearestNeedInteraction(const FVector& RefPosition, const ENeedType NeedType)
{
	ANeedModifierInteraction* NearestNeedIntersection = nullptr;
	float SqMinDist = std::numeric_limits<float>::max();
	
	for (ANeedModifierInteraction* NeedInteraction : ANeedModifierInteraction::GetInstances())
	{
		if (!NeedInteraction)
			continue;
		
		if (NeedInteraction->GetAffectedType() != NeedType)
			continue;
		
		if (NeedInteraction->GetNeedDelta() >= 0)
			continue;;
		
		if (NeedInteraction->IsInteracting())
			continue;
		
		const float SqDist = FVector::DistSquared(RefPosition, NeedInteraction->GetActorLocation());
		if (SqDist < SqMinDist)
		{
			SqMinDist = SqDist;
			NearestNeedIntersection = NeedInteraction;
		}
	}
	return NearestNeedIntersection;
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
