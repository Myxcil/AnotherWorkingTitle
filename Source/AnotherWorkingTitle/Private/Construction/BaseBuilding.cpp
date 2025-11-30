// (c) 2025 MK


#include "Construction/BaseBuilding.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	TArray<ABaseBuilding*> AllBaseBuildings;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const TArray<ABaseBuilding*>& ABaseBuilding::GetInstances()
{
	return AllBaseBuildings;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ABaseBuilding::ABaseBuilding()
{
	PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();
	AllBaseBuildings.Add(this);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ABaseBuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AllBaseBuildings.Remove(this);
	Super::EndPlay(EndPlayReason);
}
