// (c) 2025 MK


#include "Construction/BuildingSite.h"

#include "AI/AIHelper.h"
#include "AnotherWorkingTitle/AnotherWorkingTitle.h"
#include "Construction/BaseBuilding.h"
#include "Construction/BuildingDefinition.h"
#include "Inventory/Stockpile.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace 
{
	TArray<ABuildingSite*> AllBuildingSites;	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FGlobalBuildingSiteChanged ABuildingSite::OnGlobalBuildingSiteChanged;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const TArray<ABuildingSite*>& ABuildingSite::GetInstances()
{
	return AllBuildingSites;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ABuildingSite::ABuildingSite()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABuildingSite::BeginPlay()
{
	Super::BeginPlay();
	AllBuildingSites.Add(this);
	for (AStockpile* Stockpile : AStockpile::GetInstances())
	{
		Stockpile->OnInventoryChanged.AddDynamic(this, &ThisClass::OnStockpileUpdated);
	}
	OnStockpileUpdated();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABuildingSite::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (AStockpile* Stockpile : AStockpile::GetInstances())
	{
		Stockpile->OnInventoryChanged.RemoveDynamic(this, &ThisClass::OnStockpileUpdated);
	}
	AllBuildingSites.Remove(this);
	Super::EndPlay(EndPlayReason);
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
bool ABuildingSite::CanConstruct() const
{
	if (!bIsBuildStarted)
	{
		return HasResourcesForBuild();
	}
	return BuildProgress < 1.0f;
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
			bIsBuildStarted = true;
			
			if (!TryConsumeBuildResources())
			{
				UE_LOG(LogAWT, Log, TEXT("%s not enough resources"), *GetName());
				bIsBuildStarted = false;
				return false;
			}
			
			UE_LOG(LogAWT, Log, TEXT("%s begin build"), *GetName());
		}
		
		BuildProgress = FAIHelper::Clamp01(BuildProgress + Amount);
		
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
			return false;
		}
	}
	return true;
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
	
	OnGlobalBuildingSiteChanged.Broadcast();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABuildingSite::OnStockpileUpdated()
{
	if (bIsBuildStarted)
		return;
	
	TArray<FResourceAmount> Requirements = BuildingDefinition->BuildCost.Requirements;
	for (int32 I=Requirements.Num()-1; I >= 0; --I)
	{
		FResourceAmount& Requirement = Requirements[I];
		for (const AStockpile* Stockpile : AStockpile::GetInstances())
		{
			const FSettlementStock& SettlementStock = Stockpile->GetSettlementStock();
			const int32 StoredAmount = SettlementStock.GetAmount(Requirement.Resource);
			const int32 ToRemove = FMath::Min(Requirement.Amount, StoredAmount);
			Requirement.Amount -= ToRemove;
		}
		
		if (Requirement.Amount == 0)
		{
			Requirements.RemoveAt(I);
		}
	}
	
	OnBuildCostUpdated.Broadcast(Requirements);
}
