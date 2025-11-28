// (c) 2025 MK


#include "Settlers/SettlerCharacter.h"

#include "AI/GOAPAgentComponent.h"
#include "AnotherWorkingTitle/AnotherWorkingTitle.h"
#include "GameFramework/PawnMovementComponent.h"
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

	NeedsComponent->OnDamagedReachedMaximum.BindUObject(this, &ThisClass::OnDamagedReachedMaximum);	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	NeedsComponent->OnDamagedReachedMaximum.Unbind();
	
	Super::EndPlay(EndPlayReason);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsDead)
		return;
	
	if (BusyTimerAfterUse > 0)
	{
		BusyTimerAfterUse = FMath::Max(0.0f, BusyTimerAfterUse - DeltaTime);
	}
	
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
	if (BusyTimerAfterUse > 0)
		return true;
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FVector ASettlerCharacter::GetFeetPosition() const
{
	return GetMovementComponent()->GetFeetLocation();
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::UseItemInSlot(const int32 SlotIndex)
{
	check(InventoryComponent);
	check(NeedsComponent);
	
	const FInventory& Inventory = InventoryComponent->GetInventory();
	if (!Inventory.Stacks.IsValidIndex(SlotIndex))
		return;
	
	const FResourceStack& Stack = Inventory.Stacks[SlotIndex];
	check(Stack.Resource);
	const UResourceDefinition* Resource = Stack.Resource;
	if (Resource->UseTimeInSeconds <= 0)
		return;

	BusyTimerAfterUse = Resource->UseTimeInSeconds;
	
	bool bConsumeResource = false;
	
	bConsumeResource |= NeedsComponent->ChangeNeedValue(ENeedType::Hunger, Resource->HungerChange);
	bConsumeResource |= NeedsComponent->ChangeNeedValue(ENeedType::Thirst, Resource->ThirstChange);
	bConsumeResource |= NeedsComponent->ChangeNeedValue(ENeedType::Cold, Resource->ColdChange);
	bConsumeResource |= NeedsComponent->ChangeNeedValue(ENeedType::Fatigue, Resource->FatigueChange);
	bConsumeResource |= NeedsComponent->ChangeNeedValue(ENeedType::Damage, Resource->DamageChange);
	
	if (bConsumeResource)
	{
		InventoryComponent->RemoveResource(SlotIndex, 1);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::OnDamagedReachedMaximum()
{
	UE_LOG(LogAWT, Warning, TEXT("%s is DEAD!"), *GetName());
	bIsDead = true;
	OnSettlerDied.Broadcast();
}
