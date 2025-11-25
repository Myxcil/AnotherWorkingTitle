// (c) 2025 MK


#include "Settlers/NeedsComponent.h"


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
	SetNeedValue(NeedType, FMath::Clamp(NewValue, 0.0f, 1.0f));
}
