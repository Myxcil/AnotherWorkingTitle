// (c) 2025 MK


#include "Settlers/NeedsComponent.h"

#include "AI/AIHelper.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UNeedsComponent::UNeedsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UNeedsComponent::BeginPlay()
{
	Super::BeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UNeedsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Needs.Damage >= 1.0f)
		return;
	
	constexpr float SecondsPerInGameHour = 60.0f;
	const float DeltaTimeInGame = DeltaTime / SecondsPerInGameHour;
	
	Needs.Hunger = FAIHelper::Clamp01( Needs.Hunger + HungerRatePerHour * DeltaTimeInGame);
	Needs.Thirst = FAIHelper::Clamp01( Needs.Thirst + ThirstRatePerHour * DeltaTimeInGame);
	Needs.Cold = FAIHelper::Clamp01( Needs.Cold + ColdRatePerHour * DeltaTimeInGame);
	Needs.Fatigue = FAIHelper::Clamp01( Needs.Fatigue + FatigueRatePerHour * DeltaTimeInGame);

	float TotalDamage = 0;
	if (IsNeedCritical(ENeedType::Hunger))
	{
		TotalDamage += DamageForCriticalHunger;
	}
	if (IsNeedCritical(ENeedType::Thirst))
	{
		TotalDamage += DamageForCriticalThirst;
	}
	if (IsNeedCritical(ENeedType::Cold))
	{
		TotalDamage += DamageForCriticalCold;
	}
	if (IsNeedCritical(ENeedType::Fatigue))
	{
		TotalDamage += DamageForCriticalFatigue;
	}
	
	if (TotalDamage > 0)
	{
		Needs.Damage = FAIHelper::Clamp01( Needs.Damage + TotalDamage * DeltaTimeInGame);
		if (Needs.Damage >= 1.0f)
		{
			OnDamagedReachedMaximum.ExecuteIfBound();
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UNeedsComponent::SetNeedValue(const ENeedType NeedType, const float InValue)
{
	switch (NeedType)
	{
	case ENeedType::Hunger:
		Needs.Hunger = InValue;
		break;
	case ENeedType::Thirst:
		Needs.Thirst = InValue;
		break;
	case ENeedType::Cold:
		Needs.Cold = InValue;
		break;
	case ENeedType::Fatigue:
		Needs.Fatigue = InValue;
		break;
	case ENeedType::Damage:
		Needs.Damage = InValue;
		break;
	default: 
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float UNeedsComponent::GetNeedValue(const ENeedType NeedType) const
{
	switch (NeedType) 
	{
	case ENeedType::Hunger:		return Needs.Hunger;
	case ENeedType::Thirst:		return Needs.Thirst;
	case ENeedType::Cold:		return Needs.Cold;
	case ENeedType::Fatigue:	return Needs.Fatigue;
	case ENeedType::Damage:		return Needs.Damage;
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UNeedsComponent::ChangeNeedValue(const ENeedType NeedType, const float Delta)
{
	const float OldValue = GetNeedValue(NeedType);
	const float NewValue = OldValue + Delta;
	const float ClampedValue = FAIHelper::Clamp01(NewValue);
	SetNeedValue(NeedType, ClampedValue);
	return OldValue != GetNeedValue(NeedType);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UNeedsComponent::IsAnyNeedUncomfortable() const
{
	for (const ENeedType Type : TEnumRange<ENeedType>())
	{
		if (IsNeedUncomfortable(Type))
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UNeedsComponent::IsAnyNeedCritical() const
{
	for (const ENeedType Type : TEnumRange<ENeedType>())
	{
		if (IsNeedCritical(Type))
		{
			return true;
		}
	}
	return false;
}
