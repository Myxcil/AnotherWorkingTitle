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

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FText UGameTimeSubsystem::GetGameTimeFormatted() const
{
	const int32 Hours = FMath::FloorToInt(AccumulatedDayTimeHours);
	const int32 Minutes = FMath::FloorToInt((AccumulatedDayTimeHours - Hours) * 60.0f);
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Hours, Minutes));
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float UGameTimeSubsystem::GetGameHourToRealSeconds(const float InGameHour) const
{
	return InGameHour / GameHoursPerRealSecond;
}
