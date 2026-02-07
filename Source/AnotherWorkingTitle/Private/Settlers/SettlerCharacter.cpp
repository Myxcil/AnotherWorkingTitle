// (c) 2025 MK


#include "Settlers/SettlerCharacter.h"

#include "GameTimeSubsystem.h"
#include "AI/GOAPAgentComponent.h"
#include "AnotherWorkingTitle/AnotherWorkingTitle.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interactive/Interaction.h"
#include "Inventory//InventoryComponent.h"
#include "Settlers/NeedsComponent.h"
#include "Settlers/SocialComponent.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
ASettlerCharacter::ASettlerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	NeedsComponent = CreateDefaultSubobject<UNeedsComponent>(TEXT("NeedsComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	GOAPAgentComponent = CreateDefaultSubobject<UGOAPAgentComponent>(TEXT("GOAPAgentComponent"));
	SocialComponent = CreateDefaultSubobject<USocialComponent>(TEXT("SocialComponent"));
 }

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GameTimeSubsystemPtr = GetWorld()->GetSubsystem<UGameTimeSubsystem>();
	
	NeedsComponent->OnDamagedReachedMaximum.BindUObject(this, &ThisClass::OnDamagedReachedMaximum);
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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
	
	const UGameTimeSubsystem* GameTimeSubsystem = GameTimeSubsystemPtr.Get();
	if (!GameTimeSubsystem)
		return;
	
	const float GameDeltaTime = GameTimeSubsystem->GetGameDeltaHour();
	
	if (BusyTimerAfterUse > 0)
	{
		BusyTimerAfterUse = FMath::Max(0.0f, BusyTimerAfterUse - DeltaTime);
	}
	
	if (NeedsComponent)
	{
		NeedsComponent->TickNeeds(GameDeltaTime);
	}
	
	if (SocialComponent)
	{
		SocialComponent->TickSocial(GameDeltaTime);
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
void ASettlerCharacter::SetSprinting(const bool bEnable)
{
	if (bIsSprinting == bEnable)
		return;
	
	bIsSprinting = bEnable;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? RunSpeed : WalkSpeed;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ASettlerCharacter::UseSlot(const int32 SlotIndex)
{
	check(InventoryComponent);
	check(NeedsComponent);
	
	const FInventory& Inventory = InventoryComponent->GetInventory();
	if (!Inventory.Stacks.IsValidIndex(SlotIndex))
		return false;
	
	const FResourceStack& Stack = Inventory.Stacks[SlotIndex];
	check(Stack.Resource);
	const UResourceDefinition* Resource = Stack.Resource;
	if (Resource->UseTimeInSeconds <= 0)
		return false;

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
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::TryBeginInteract(AActor* Target)
{
	if (!Target)
		return;
	
	if (CurrentHoldInteraction.IsValid())
		return;
	
	const UClass* TargetClass = Target->GetClass();
	check(TargetClass);
	
	if (TargetClass->ImplementsInterface(UHoldInteraction::StaticClass()))
	{
		CurrentHoldInteraction = Target;
		IHoldInteraction::Execute_BeginInteraction(Target, this);
	}
	else if (TargetClass->ImplementsInterface(UInteraction::StaticClass()))
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
	UseSlot(SlotIndex);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerCharacter::OnDamagedReachedMaximum()
{
	UE_LOG(LogAWT, Warning, TEXT("%s is DEAD!"), *GetName());
	bIsDead = true;
	OnSettlerDied.Broadcast();
}
