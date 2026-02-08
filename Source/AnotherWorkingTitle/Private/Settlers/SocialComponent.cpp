// (c) 2025 MK


#include "Settlers/SocialComponent.h"

#include "AnotherWorkingTitle/AnotherWorkingTitle.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	TArray<USocialComponent*> AllSocialComponents;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USocialComponent::USocialComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (EmotionalArchetype)
	{
		Temperament = EmotionalArchetype->Temperament;
		Thresholds = EmotionalArchetype->Thresholds;
		BaselineEmotion = EmotionalArchetype->BaselineEmotion;
		GlobalIntensity = EmotionalArchetype->GlobalIntensity;
	}
	
	CurrentEmotion = BaselineEmotion;
	CachedSummary.Evaluate(CurrentEmotion, Thresholds);
	
	for (USocialComponent* Other : AllSocialComponents)
	{
		check(Other != this);
		Connect(Other);
		Other->Connect(this);
	}
	AllSocialComponents.Add(this);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AllSocialComponents.Remove(this);
	for (USocialComponent* Other : AllSocialComponents)
	{
		check(Other != this);
		Disconnect(Other);
		Other->Disconnect(this);
	}
	Super::EndPlay(EndPlayReason);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::Connect(USocialComponent* SocialComponent)
{
	check(SocialComponent);
	check(!RelationShips.Contains(SocialComponent));
	RelationShips.Add(SocialComponent, FRelationshipState());
	UE_LOG(LogAWT, Log, TEXT("Social Connection: %s -> %s"), *GetOwner()->GetName(), *SocialComponent->GetOwner()->GetName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::Disconnect(USocialComponent* SocialComponent)
{
	check(SocialComponent);
	check(RelationShips.Contains(SocialComponent));
	RelationShips.Remove(SocialComponent);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::TickSocial(const float DeltaGameHour)
{
	UpdateEmotionRegulation(DeltaGameHour);
	if (bIsDirty)
	{
		bIsDirty = false;

		FEmotionSummary NewSummary;
		NewSummary.Evaluate(CurrentEmotion, Thresholds);
		if (NewSummary != CachedSummary)
		{
			CachedSummary = NewSummary;
			OnEmotionalStateChanged.Broadcast();
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FText USocialComponent::GetEmotionalState() const
{
	FString Output;
	if (CachedSummary.JoySadness != EEmotionalLevel::Neutral)
	{
		Output.Appendf(TEXT("%s "), *GetEmotionDescription(EPrimaryEmotionAxis::JoySadness, CachedSummary.JoySadness));	
	}
	if (CachedSummary.TrustDisgust != EEmotionalLevel::Neutral)
	{
		Output.Appendf(TEXT("%s "), *GetEmotionDescription(EPrimaryEmotionAxis::TrustDisgust, CachedSummary.TrustDisgust));	
	}
	if (CachedSummary.FearAnger != EEmotionalLevel::Neutral)
	{
		Output.Appendf(TEXT("%s "), *GetEmotionDescription(EPrimaryEmotionAxis::FearAnger, CachedSummary.FearAnger));	
	}
	if (CachedSummary.SurpriseAnticipation != EEmotionalLevel::Neutral)
	{
		Output.Appendf(TEXT("%s "), *GetEmotionDescription(EPrimaryEmotionAxis::SurpriseAnticipation, CachedSummary.SurpriseAnticipation));	
	}
	return FText::FromString(Output);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::QueryEmotionalState(FString& Output) const
{
	if (CachedSummary.JoySadness != EEmotionalLevel::Neutral)
	{
		Output.Append(GetEmotionDescription(EPrimaryEmotionAxis::JoySadness, CachedSummary.JoySadness));	
	}
	if (CachedSummary.TrustDisgust != EEmotionalLevel::Neutral)
	{
		if (!Output.IsEmpty()) Output.AppendChar(',');
		Output.Append(GetEmotionDescription(EPrimaryEmotionAxis::TrustDisgust, CachedSummary.TrustDisgust));	
	}
	if (CachedSummary.FearAnger != EEmotionalLevel::Neutral)
	{
		if (!Output.IsEmpty()) Output.AppendChar(',');
		Output.Append(GetEmotionDescription(EPrimaryEmotionAxis::FearAnger, CachedSummary.FearAnger));	
	}
	if (CachedSummary.SurpriseAnticipation != EEmotionalLevel::Neutral)
	{
		if (!Output.IsEmpty()) Output.AppendChar(',');
		Output.Append(GetEmotionDescription(EPrimaryEmotionAxis::SurpriseAnticipation, CachedSummary.SurpriseAnticipation));	
	}
	if (!Output.IsEmpty())
	{
		Output.InsertAt(0, "You feel ");
		Output.Append(".\n");
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool USocialComponent::QueryRelationship(FString& Output, const USocialComponent* Other) const
{
	if (const FRelationshipState* PtrRelationshipState = RelationShips.Find(Other))
	{
		Output = GetRelationShipDescription(*PtrRelationshipState);
		if (!Output.IsEmpty())
		{
			Output.InsertAt(0, "Your attitude towards the player is ");
			Output.Append(".\n");
		}
		return true;
	}
	return false;
}

#if WITH_EDITOR
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::RandomizeCurrentEmotion()
{
	for (const EPrimaryEmotionAxis Axis : TEnumRange<EPrimaryEmotionAxis>())
	{
		CurrentEmotion.SetValue(Axis, FMath::FRandRange(-1.0f, 1.0f));
	}
	bIsDirty = true;
}
#endif

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::ApplyEmotion(const EPrimaryEmotionAxis EmotionAxis, const float Delta)
{
	const FEmotionAxisTemperament& Axis = Temperament.GetAxis(EmotionAxis);
	const float BiasedDelta = Delta * (1.0f + Axis.Bias);
	const float ScaledDelta = BiasedDelta * Axis.Reactivity;
	if (CurrentEmotion.ChangeValue(EmotionAxis, ScaledDelta * GlobalIntensity))
	{
		bIsDirty = true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::UpdateEmotionRegulation(const float DeltaGameHour)
{
	for (EPrimaryEmotionAxis EmotionAxis : TEnumRange<EPrimaryEmotionAxis>())
	{
		const FEmotionAxisTemperament& AxisTemperament = Temperament.GetAxis(EmotionAxis);
		const float Value = CurrentEmotion.GetValue(EmotionAxis);
		const float Baseline = BaselineEmotion.GetValue(EmotionAxis);
		
		const float Decay = AxisTemperament.Regulation * DeltaGameHour;
		const float NewValue = Value + ((Baseline - Value) * Decay);
		
		CurrentEmotion.SetValue(EmotionAxis, NewValue);
		
		bIsDirty |=(NewValue != Value);
	}
}
