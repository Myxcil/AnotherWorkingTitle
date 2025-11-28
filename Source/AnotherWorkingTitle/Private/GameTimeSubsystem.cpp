// (c) 2025 MK


#include "GameTimeSubsystem.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGameTimeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	AccumulatedDayTimeHours = 0;
	GameDay = 0;
	LastGameDeltaHour = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGameTimeSubsystem::OnWorldEndPlay(UWorld& InWorld)
{
	Super::OnWorldEndPlay(InWorld);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UGameTimeSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	LastGameDeltaHour = DeltaTime * GameHoursPerRealSecond;
	AccumulatedDayTimeHours += LastGameDeltaHour;
	
	if (AccumulatedDayTimeHours >= 24.0f)
	{
		AccumulatedDayTimeHours -= 24.0f;
		++GameDay;
		OnGameDayChanged.Broadcast();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
TStatId UGameTimeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGameTimeSubsystem, STATGROUP_Tickables);
}
