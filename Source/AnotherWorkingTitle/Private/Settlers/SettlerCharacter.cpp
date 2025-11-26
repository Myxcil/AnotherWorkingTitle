// (c) 2025 MK


#include "Settlers/SettlerCharacter.h"

#include "AI/GOAPAgentComponent.h"
#include "Interactive/Interaction.h"
#include "Inventory//InventoryComponent.h"
#include "Settlers/NeedsComponent.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
ASettlerCharacter::ASettlerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	NeedsComponent = CreateDefaultSubobject<UNeedsComponent>(TEXT("NeedsComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
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
	
	if (AActor* Target = CurrentHoldInteraction.Get())
	{
		if (!IHoldInteraction::Execute_TickInteraction(Target, this, DeltaTime))
		{
			TryEndInteract();
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ASettlerCharacter::IsBusy() const
{
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::TryBeginInteract(AActor* Target)
{
	if (!Target)
		return;
	
	if (CurrentHoldInteraction.IsValid())
		return;
	
	if (Cast<IHoldInteraction>(Target))
	{
		CurrentHoldInteraction = Target;
		IHoldInteraction::Execute_BeginInteraction(Target, this);
	}
	else if (Cast<IInteraction>(Target))
	{
		IInteraction::Execute_Interact(Target, this);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::TryEndInteract()
{
	AActor* Target = CurrentHoldInteraction.Get();
	if (!Target)
		return;
	
	IHoldInteraction::Execute_EndInteraction(Target, this);
	
	CurrentHoldInteraction.Reset();
}
