// (c) 2025 MK


#include "Settlers/SocialComponent.h"

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
		BaselineEmotion = EmotionalArchetype->BaselineEmotion;
		GlobalIntensity = EmotionalArchetype->GlobalIntensity;
	}
	
	CurrentEmotion = BaselineEmotion;
	
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
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::ApplyEmotion(const EPrimaryEmotionAxis EmotionAxis, const float Delta)
{
	const FEmotionAxisTemperament& Axis = Temperament.GetAxis(EmotionAxis);
	const float BiasedDelta = Delta * (1.0f + Axis.Bias);
	const float ScaledDelta = BiasedDelta * Axis.Reactivity;
	CurrentEmotion.ChangeValue(EmotionAxis, ScaledDelta * GlobalIntensity);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void USocialComponent::UpdateEmotionRegulation(const float DeltaGameHour)
{
	for (EPrimaryEmotionAxis EmotionAxis : TEnumRange<EPrimaryEmotionAxis>())
	{
		const FEmotionAxisTemperament& AxisTemperament = Temperament.GetAxis(EmotionAxis);
		float Value = CurrentEmotion.GetValue(EmotionAxis);
		const float Baseline = BaselineEmotion.GetValue(EmotionAxis);
		
		const float Decay = AxisTemperament.Regulation * DeltaGameHour;
		Value += (Baseline - Value) * Decay;
		
		CurrentEmotion.SetValue(EmotionAxis, Value);
	}
}
