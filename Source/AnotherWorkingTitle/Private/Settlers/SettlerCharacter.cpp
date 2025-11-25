// (c) 2025 MK


#include "Settlers/SettlerCharacter.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
ASettlerCharacter::ASettlerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
