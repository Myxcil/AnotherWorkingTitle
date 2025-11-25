// (c) 2024 by Crenetic GmbH Studios

//------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "AI/AIHelper.h"

#include "AI/AIConstants.h"
#include "Inventory/Stockpile.h"
#include "Resources/ResourceNode.h"
#include "Resources/ResourceRegistrySubsystem.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FAIHelper::GetObjectTransform(const UObject* Object, FTransform& Result)
{
	check(Object);
	
	if (const AActor* Actor = Cast<AActor>(Object))
	{
		Result = Actor->GetActorTransform();
		return true;
	}

	if (const USceneComponent* SceneComponent = Cast<USceneComponent>(Object))
	{
		Result = SceneComponent->GetComponentTransform();
		return true;
	}

	AI_WARN(TEXT("GetObjectTransform, unhandled object %s (%s)"), *Object->GetName(), *Object->GetClass()->GetName());
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
AStockpile* FAIHelper::FindNearestStockpile(const FVector& RefPosition)
{
	const TArray<AStockpile*>& AllStockpiles = AStockpile::GetInstances();
	if (AllStockpiles.Num() == 0)
		return nullptr;
	
	AStockpile* NearestPile = nullptr;
	float SqMinDist = std::numeric_limits<float>::max();
	
	for (AStockpile* Stockpile : AllStockpiles)
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
