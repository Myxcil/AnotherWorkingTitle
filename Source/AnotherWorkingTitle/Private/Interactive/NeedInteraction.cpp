// (c) 2025 MK


#include "Interactive/NeedInteraction.h"

#include "GameTimeSubsystem.h"
#include "Settlers/NeedsComponent.h"
#include "Settlers/SettlerCharacter.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace 
{
	TArray<ANeedInteraction*> AllNeedInteractions;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const TArray<ANeedInteraction*>& ANeedInteraction::GetInstances()
{
	return AllNeedInteractions;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ANeedInteraction::ANeedInteraction()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedInteraction::BeginPlay()
{
	Super::BeginPlay();
	GameTimeSubsystemPtr = GetWorld()->GetSubsystem<UGameTimeSubsystem>();
	AllNeedInteractions.Add(this);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedInteraction::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AllNeedInteractions.Remove(this);
	Super::EndPlay(EndPlayReason);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedInteraction::BeginInteraction_Implementation(ASettlerCharacter* SettlerCharacter)
{
	CurrentUser = SettlerCharacter;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedInteraction::EndInteraction_Implementation(ASettlerCharacter* SettlerCharacter)
{
	StopInteraction();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ANeedInteraction::TickInteraction_Implementation(ASettlerCharacter* SettlerCharacter, float DeltaTime)
{
	return UpdateInteraction();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float ANeedInteraction::StartInteraction(ASettlerCharacter* Settler)
{
	if (CurrentUser.IsValid())
		return false;
	
	const UGameTimeSubsystem* GameTimeSubsystem = GameTimeSubsystemPtr.Get();
	if (!GameTimeSubsystem)
		return false;
	
	CurrentUser = Settler;
	SetActorTickEnabled(true);

	float DurationRealTime = 0;;
	if (NeedsValueDelta < 0)
	{
		if (const UNeedsComponent* NeedsComponent = Settler->GetNeedsComponent())
		{
			const float NeedValue = NeedsComponent->GetNeedValue(AffectedType);
			const float DurationToFull = NeedValue / -NeedsValueDelta;
			DurationRealTime = GameTimeSubsystem->GetGameHourToRealSeconds(DurationToFull);
		}
	}
	return FMath::Max(5.0f, DurationRealTime);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedInteraction::StopInteraction()
{
	CurrentUser.Reset();
	if (IsActorTickEnabled())
	{
		SetActorTickEnabled(false);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedInteraction::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!UpdateInteraction())
	{
		StopInteraction();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ANeedInteraction::UpdateInteraction()
{
	const UGameTimeSubsystem* GameTimeSubsystem = GameTimeSubsystemPtr.Get();
	if (!GameTimeSubsystem)
		return false;

	const ASettlerCharacter* User = CurrentUser.Get();
	if (!User)
		return false;
	
	UNeedsComponent* NeedsComponent = User->GetNeedsComponent();
	check(NeedsComponent);
	
	const float DeltaChange = NeedsValueDelta * GameTimeSubsystem->GetGameDeltaHour();
	if (!NeedsComponent->ChangeNeedValue(AffectedType, DeltaChange))
	{
		return false;
	}
	
	return true;
}
