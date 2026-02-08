// (c) 2025 MK


#include "Settlers/NeedsComponent.h"

#include "AI/AIHelper.h"
#include "Settlers/SettlerCharacter.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UNeedsComponent::UNeedsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UNeedsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SettlerCharacterPtr = Cast<ASettlerCharacter>(GetOwner());
	
	for (int32 I=0; I < PrevNeedSeverity.Num(); ++I)
	{
		PrevNeedSeverity[I] = ENeedSeverity::Normal;		
	}	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UNeedsComponent::TickNeeds(const float DeltaGameHour)
{
	if (DeltaGameHour == 0)
		return;
	
	if (!bUndead && Needs.Damage >= 1.0f)
		return;
	
	if (bInvincible)
	{
		for (const ENeedType NeedType : TEnumRange<ENeedType>())
		{
			if (GetNeedValue(NeedType) > 0.0f)
			{
				SetNeedValue(NeedType, 0.0f);
			}
		}
	}
	else
	{
		Needs.Hunger = FAIHelper::Clamp01( Needs.Hunger + HungerRatePerHour * DeltaGameHour);
		Needs.Thirst = FAIHelper::Clamp01( Needs.Thirst + ThirstRatePerHour * DeltaGameHour);
		Needs.Cold = FAIHelper::Clamp01( Needs.Cold + ColdRatePerHour * DeltaGameHour);
		
		float FatigueRate = FatigueRatePerHour;
		if (SettlerCharacterPtr->IsSprinting())
		{
			FatigueRate *= FatigueRateScaleSprint;
		}
		Needs.Fatigue = FAIHelper::Clamp01( Needs.Fatigue + FatigueRate * DeltaGameHour);
	}
	
	float TotalDamage = 0;
	if (CalculateSeverity(ENeedType::Hunger) == ENeedSeverity::Critical)
	{
		TotalDamage += DamageForCriticalHunger;
	}
	if (CalculateSeverity(ENeedType::Thirst) == ENeedSeverity::Critical)
	{
		TotalDamage += DamageForCriticalThirst;
	}
	if (CalculateSeverity(ENeedType::Cold) == ENeedSeverity::Critical)
	{
		TotalDamage += DamageForCriticalCold;
	}
	if (CalculateSeverity(ENeedType::Fatigue) == ENeedSeverity::Critical)
	{
		TotalDamage += DamageForCriticalFatigue;
	}
	
	if (TotalDamage > 0)
	{
		Needs.Damage = FAIHelper::Clamp01( Needs.Damage + TotalDamage * DeltaGameHour);
		if (!bUndead && Needs.Damage >= 1.0f)
		{
			OnDamagedReachedMaximum.ExecuteIfBound();
		}
	}
	
	CheckNeedSeverity();
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
ENeedSeverity UNeedsComponent::CalculateSeverity(const ENeedType NeedType) const
{
	const float Value = GetNeedValue(NeedType);
	if (Value >= CriticalThreshold)
	{
		return ENeedSeverity::Critical;
	}
	if (Value >= UncomfortableThreshold)
	{
		return ENeedSeverity::Uncomfortable;
	}
	return ENeedSeverity::Normal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UNeedsComponent::IsAnyNeedInSeverityLevel(const ENeedSeverity NeedSeverity) const
{
	for (const ENeedType Type : TEnumRange<ENeedType>())
	{
		if (CalculateSeverity(Type) == NeedSeverity)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UNeedsComponent::QueryState(FString& Output) const
{
	int32 Index = 0;
	bool bNeedAdded = false;
	for (const ENeedType NeedType : TEnumRange<ENeedType>())
	{
		if (PrevNeedSeverity[Index] >= ENeedSeverity::Uncomfortable)
		{
			if (bNeedAdded)
			{
				Output.AppendChar(',');
			}
			
			const bool bCritical = PrevNeedSeverity[Index] == ENeedSeverity::Critical;
		
			switch (NeedType)
			{
			case ENeedType::Hunger:
				Output.Append(bCritical ? "hungry" : "starving");
				bNeedAdded = true;
				break;
			case ENeedType::Thirst:
				Output.Append(bCritical ? "thirsty" : "dehydrated");
				bNeedAdded = true;
				break;
			case ENeedType::Cold:
				Output.Append(bCritical ? "cold" : "freezing");
				bNeedAdded = true;
				break;
			case ENeedType::Fatigue:
				Output.Append(bCritical ? "tired" : "exhausted");
				bNeedAdded = true;
				break;
			case ENeedType::Damage:
				Output.Append(bCritical ? "hurt" : "dying");
				bNeedAdded = true;
				break;
			}
		}
		++Index;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UNeedsComponent::CheckNeedSeverity()
{
	int32 Index = 0;
	for (const ENeedType NeedType : TEnumRange<ENeedType>())
	{
		const ENeedSeverity NewSeverity = CalculateSeverity(NeedType);
		if (PrevNeedSeverity[Index] != NewSeverity)
		{
			PrevNeedSeverity[Index] = NewSeverity;
			OnNeedSeverityChanged.Broadcast();
		}
		++Index;
	}
}
