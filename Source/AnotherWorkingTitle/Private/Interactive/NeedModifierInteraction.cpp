// (c) 2025 MK


#include "Interactive/NeedModifierInteraction.h"

#include "Settlers/NeedsComponent.h"
#include "Settlers/SettlerCharacter.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace 
{
	TArray<ANeedModifierInteraction*> AllNeedInteractions;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const TArray<ANeedModifierInteraction*>& ANeedModifierInteraction::GetInstances()
{
	return AllNeedInteractions;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ANeedModifierInteraction::ANeedModifierInteraction()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedModifierInteraction::BeginPlay()
{
	Super::BeginPlay();
	AllNeedInteractions.Add(this);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedModifierInteraction::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AllNeedInteractions.Remove(this);
	Super::EndPlay(EndPlayReason);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedModifierInteraction::BeginInteraction_Implementation(ASettlerCharacter* SettlerCharacter)
{
	CurrentUser = SettlerCharacter;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedModifierInteraction::EndInteraction_Implementation(ASettlerCharacter* SettlerCharacter)
{
	StopInteraction();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ANeedModifierInteraction::TickInteraction_Implementation(ASettlerCharacter* SettlerCharacter, float DeltaTime)
{
	return UpdateInteraction();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float ANeedModifierInteraction::StartInteraction(ASettlerCharacter* Settler)
{
	if (CurrentUser.IsValid())
		return -1.0f;
	
	CurrentUser = Settler;
	SetActorTickEnabled(true);

	float DurationRealTime = 0;;
	if (NeedsValueDelta < 0)
	{
		if (const UNeedsComponent* NeedsComponent = Settler->GetNeedsComponent())
		{
			const float NeedValue = NeedsComponent->GetNeedValue(AffectedType);
			const float DurationToFull = NeedValue / -NeedsValueDelta;
			DurationRealTime = GetGameHourToRealSeconds(DurationToFull);
		}
	}
	return FMath::Max(5.0f, DurationRealTime);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedModifierInteraction::StopInteraction()
{
	CurrentUser.Reset();
	if (IsActorTickEnabled())
	{
		SetActorTickEnabled(false);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedModifierInteraction::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!UpdateInteraction())
	{
		StopInteraction();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ANeedModifierInteraction::UpdateInteraction()
{
	const ASettlerCharacter* User = CurrentUser.Get();
	if (!User)
		return false;
	
	UNeedsComponent* NeedsComponent = User->GetNeedsComponent();
	check(NeedsComponent);
	
	const float DeltaChange = NeedsValueDelta * GetDeltaTimeHour();
	if (!NeedsComponent->ChangeNeedValue(AffectedType, DeltaChange))
	{
		return false;
	}
	
	return true;
}
