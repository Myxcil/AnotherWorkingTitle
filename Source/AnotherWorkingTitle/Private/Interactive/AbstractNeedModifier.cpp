// (c) 2025 MK


#include "Interactive/AbstractNeedModifier.h"

#include "GameTimeSubsystem.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
AAbstractNeedModifier::AAbstractNeedModifier()
{
	PrimaryActorTick.bCanEverTick = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void AAbstractNeedModifier::BeginPlay()
{
	Super::BeginPlay();
	GameTimeSubsystemPtr = GetWorld()->GetSubsystem<UGameTimeSubsystem>();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float AAbstractNeedModifier::GetDeltaTimeHour() const
{
	if (const UGameTimeSubsystem* GameTimeSubsystem = GameTimeSubsystemPtr.Get())
	{
		return GameTimeSubsystem->GetGameDeltaHour();
	}
	return 0;
}

float AAbstractNeedModifier::GetGameHourToRealSeconds(const float GameHour) const
{
	if (const UGameTimeSubsystem* GameTimeSubsystem = GameTimeSubsystemPtr.Get())
	{
		return GameTimeSubsystem->GetGameHourToRealSeconds(GameHour);
	}
	return 0;
}
