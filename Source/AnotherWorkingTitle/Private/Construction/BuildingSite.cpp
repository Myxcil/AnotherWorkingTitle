// (c) 2025 MK


#include "Construction/BuildingSite.h"

#include "AnotherWorkingTitle/AnotherWorkingTitle.h"
#include "Construction/BuildingDefinition.h"
#include "Inventory/Stockpile.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
ABuildingSite::ABuildingSite()
{
	PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABuildingSite::BeginPlay()
{
	Super::BeginPlay();
	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABuildingSite::BeginInteraction_Implementation(ASettlerCharacter* SettlerCharacter)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ABuildingSite::TickInteraction_Implementation(ASettlerCharacter* SettlerCharacter, float DeltaTime)
{
	return ApplyWork(DeltaTime);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABuildingSite::EndInteraction_Implementation(ASettlerCharacter* SettlerCharacter)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ABuildingSite::HasResourcesForBuild() const
{
	if (!BuildingDefinition)
		return false;
	
	const TArray<AStockpile*>& AllStockpiles = AStockpile::GetInstances();
	if (AllStockpiles.Num() == 0)
		return false;
	
	const FBuildCost& BuildCost = BuildingDefinition->BuildCost;
	const TArray<FResourceAmount>& Requirements = BuildCost.Requirements;
	if (Requirements.Num() == 0)
		return true;

	return CanAfford(AllStockpiles, Requirements);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ABuildingSite::ApplyWork(const float Amount)
{
	if (BuildProgress < 1.0f)
	{
		if (!bIsBuildStarted)
		{
			if (!TryConsumeBuildResources())
			{
				UE_LOG(LogAWT, Log, TEXT("%s not enough resources"), *GetName());
				return false;
			}
			
			bIsBuildStarted = true;
			
			UE_LOG(LogAWT, Log, TEXT("%s begin build"), *GetName());
		}
		
		BuildProgress = FMath::Clamp(BuildProgress + Amount, 0.0f, 1.0f);
		
		OnBuildingProgress.Broadcast(BuildProgress);
		
		UE_LOG(LogAWT, Log, TEXT("%s progress = %f"), *GetName(), BuildProgress);
		
		if (BuildProgress >= 1.0f)
		{
			OnBuildCompleted();
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ABuildingSite::TryConsumeBuildResources()
{
	if (!BuildingDefinition)
		return false;
	
	const TArray<AStockpile*>& AllStockpiles = AStockpile::GetInstances();
	if (AllStockpiles.Num() == 0)
		return false;
	
	const FBuildCost& BuildCost = BuildingDefinition->BuildCost;
	const TArray<FResourceAmount>& Requirements = BuildCost.Requirements;
	if (Requirements.Num() == 0)
		return true;

	if (!CanAfford(AllStockpiles, Requirements))
		return false;

	for (const FResourceAmount& Requirement : Requirements)
	{
		if (!Requirement.Resource || Requirement.Amount == 0)
			continue;
		
		int32 RemainingAmount = Requirement.Amount;
		for (AStockpile* Stockpile : AllStockpiles)
		{
			const int32 Available = Stockpile->GetSettlementStock().GetAmount(Requirement.Resource);
			if (Available <= 0)
				continue;
			
			const int32 ToRemove = FMath::Min(Available, RemainingAmount);
			if (ToRemove <= 0)
				continue;
			
			const int32 NumRemoved = Stockpile->RemoveResource(Requirement.Resource, ToRemove);
			RemainingAmount -= NumRemoved;
			if (RemainingAmount == 0)
				break;
		}
		
		ensureMsgf(RemainingAmount == 0, TEXT("TryConsumeBuildResources: availability check and consumption got out of sync."));
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ABuildingSite::CanAfford(const TArray<AStockpile*>& AllStockpiles, const TArray<FResourceAmount>& Requirements) const
{
	for (const FResourceAmount& Requirement : Requirements)
	{
		if (!Requirement.Resource || Requirement.Amount == 0)
			continue;
		
		int32 TotalAmount = 0;
		for (const AStockpile* Stockpile : AllStockpiles)
		{
			const FSettlementStock& SettlementStock = Stockpile->GetSettlementStock();
			TotalAmount += SettlementStock.GetAmount(Requirement.Resource);
			if (TotalAmount >= Requirement.Amount)
				break;
		}
		if (TotalAmount < Requirement.Amount)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABuildingSite::OnBuildCompleted()
{
	UE_LOG(LogAWT, Log, TEXT("%s build completed"), *GetName());
	
	UWorld* World = GetWorld();
	if (!World)
		return;
	
	if (BuildingDefinition->BuildingToSpawn)
	{
		World->SpawnActor(BuildingDefinition->BuildingToSpawn, &GetTransform());
	}
	
	World->DestroyActor(this);
}
