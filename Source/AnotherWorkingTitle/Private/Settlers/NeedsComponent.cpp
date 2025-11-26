// (c) 2025 MK


#include "Settlers/NeedsComponent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace 
{
	float Clamp01(const float InValue) { return FMath::Clamp(InValue, 0.0f, 1.0f); }
}

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
	
	constexpr float SecondsPerInGameHour = 60.0f;
	const float DeltaTimeInGame = DeltaTime / SecondsPerInGameHour;
	
	Needs.Hunger = Clamp01( Needs.Hunger + HungerRatePerHour * DeltaTimeInGame);
	Needs.Thirst = Clamp01( Needs.Thirst + ThirstRatePerHour * DeltaTimeInGame);
	Needs.Fatigue = Clamp01( Needs.Fatigue + FatigueRatePerHour * DeltaTimeInGame);

	float TotalDamage = 0;
	if (Needs.Hunger >= 1.0f)
	{
		TotalDamage += DamageForCriticalHunger;
	}
	if (Needs.Thirst >= 1.0f)
	{
		TotalDamage += DamageForCriticalThirst;
	}
	if (TotalDamage > 0)
	{
		Needs.Damage = Clamp01( Needs.Damage + TotalDamage * DeltaTimeInGame);
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
void UNeedsComponent::ChangeNeedValue(const ENeedType NeedType, const float Delta)
{
	const float OldValue = GetNeedValue(NeedType);
	const float NewValue = OldValue + Delta;
	const float ClampedValue = Clamp01(NewValue);
	SetNeedValue(NeedType, ClampedValue);
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
