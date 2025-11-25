// (c) 2025 MK


#include "Settlers/SettlerCharacter.h"

#include "AI/GOAPAgentComponent.h"
#include "Interactive/WorkComponent.h"
#include "Inventory//InventoryComponent.h"
#include "Settlers/NeedsComponent.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
ASettlerCharacter::ASettlerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	NeedsComponent = CreateDefaultSubobject<UNeedsComponent>(TEXT("NeedsComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	WorkComponent = CreateDefaultSubobject<UWorkComponent>(TEXT("WorkComponent"));
	GOAPAgentComponent = CreateDefaultSubobject<UGOAPAgentComponent>(TEXT("GOAPAgentComponent"));
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
