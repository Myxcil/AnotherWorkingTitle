// (c) 2025 MK


#include "Settlers/Emotions.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FEmotionalState::SetValue(const EPrimaryEmotionAxis Axis, const float NewValue)
{
	switch (Axis)
	{
	case EPrimaryEmotionAxis::JoySadness:
		JoySadness = FMath::Clamp(NewValue, -1.0f, 1.0f);
		break;
	case EPrimaryEmotionAxis::TrustDisgust:
		TrustDisgust = FMath::Clamp(NewValue, -1.0f, 1.0f);
		break;
	case EPrimaryEmotionAxis::FearAnger:
		FearAnger = FMath::Clamp(NewValue, -1.0f, 1.0f);
		break;
	case EPrimaryEmotionAxis::SurpriseAnticipation:
		SurpriseAnticipation = FMath::Clamp(NewValue, -1.0f, 1.0f);
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
float FEmotionalState::GetValue(const EPrimaryEmotionAxis Axis) const
{
	switch (Axis)
	{
	case EPrimaryEmotionAxis::JoySadness:
		return JoySadness;
	case EPrimaryEmotionAxis::TrustDisgust:
		return TrustDisgust;
	case EPrimaryEmotionAxis::FearAnger:
		return FearAnger;
	case EPrimaryEmotionAxis::SurpriseAnticipation:
		return SurpriseAnticipation;
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FEmotionalState::ChangeValue(const EPrimaryEmotionAxis Axis, const float Delta)
{
	const float OldValue = GetValue(Axis);
	SetValue(Axis, OldValue + Delta);
	return OldValue != GetValue(Axis);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EEmotionalLevel EvaluateEmotion(const float Value, const FEmotionalThreshold& Positive, const FEmotionalThreshold& Negative)
{
	if (Value <= -Negative.High)
	{
		return EEmotionalLevel::Worst;
	}
	if (Value <= -Negative.Med)
	{
		return EEmotionalLevel::Worse;
	}
	if (Value <= -Negative.Low)
	{
		return EEmotionalLevel::Bad;
	}
	if (Value >= Positive.High)
	{
		return EEmotionalLevel::Best;
	}
	if (Value >= Positive.Med)
	{
		return EEmotionalLevel::Better;
	}
	if (Value >= Positive.Low)
	{
		return EEmotionalLevel::Good;
	}
	return EEmotionalLevel::Neutral;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FEmotionSummary::Evaluate(const FEmotionalState& EmotionalState, const FEmotionalThresholds& EmotionalThresholds)
{
	JoySadness = EvaluateEmotion(EmotionalState.JoySadness, EmotionalThresholds.Joy, EmotionalThresholds.Sadness);
	TrustDisgust = EvaluateEmotion(EmotionalState.TrustDisgust, EmotionalThresholds.Trust, EmotionalThresholds.Disgust);
	FearAnger = EvaluateEmotion(EmotionalState.FearAnger, EmotionalThresholds.Fear, EmotionalThresholds.Anger);
	SurpriseAnticipation = EvaluateEmotion(EmotionalState.SurpriseAnticipation, EmotionalThresholds.Surprise, EmotionalThresholds.Anticipation);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FRelationshipState::ApplyEmotionDelta(const FEmotionalState& Delta, float Weight, float Smoothing)
{
	for (EPrimaryEmotionAxis Axis : TEnumRange<EPrimaryEmotionAxis>())
	{
		const float Old = LongTermEmotion.GetValue(Axis);
		const float DeltaValue = Delta.GetValue(Axis) * Weight;

		// Rohwert aktualisieren (geclampter -1..+1 Bereich)
		const float RawNew = FMath::Clamp(Old + DeltaValue, -1.0f, 1.0f);

		// Glättung: bewegt sich nur ein Stück in Richtung RawNew
		const float New = FMath::Lerp(Old, RawNew, Smoothing);

		LongTermEmotion.SetValue(Axis, New);
	}

	RecomputeDerivedValues();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void FRelationshipState::RecomputeDerivedValues()
{
	const float JoySad    = LongTermEmotion.GetValue(EPrimaryEmotionAxis::JoySadness);          // + Joy, - Sadness
	const float TrustDis  = LongTermEmotion.GetValue(EPrimaryEmotionAxis::TrustDisgust);        // + Trust, - Disgust
	const float FearAnger = LongTermEmotion.GetValue(EPrimaryEmotionAxis::FearAnger);           // + Fear, - Anger
	const float SurprAnt  = LongTermEmotion.GetValue(EPrimaryEmotionAxis::SurpriseAnticipation);// + Surprise, - Anticipation

	const float DisgustPenalty = FMath::Abs(FMath::Min(TrustDis, 0.0f));
	const float AngerPenalty   = FMath::Abs(FMath::Min(FearAnger, 0.0f));
	
	// Mögen: viel Joy und Trust, wenig Disgust/Anger
	const float AffinityRaw =
		0.6f * JoySad +        // Joy -> Affinity+
		0.4f * TrustDis       // Trust -> Affinity+
		- 0.3f * DisgustPenalty
		- 0.3f * AngerPenalty;

	// Vertrauen: stark von Trust/Disgust geprägt, Angst reduziert es
	const float TrustRaw =
		0.9f * TrustDis
		- 0.3f * AngerPenalty; // Anger -> Trust-

	// Respekt: Mischung aus wahrgenommener Kompetenz/Überraschung
	// hier grob mit Surprise/Anticipation gekoppelt (jemand, der oft positiv "überrascht"/vorausschauend ist)
	const float RespectRaw =
		0.3f * TrustDis +
		0.4f * SurprAnt; // +Surprise / -Anticipation kannst du je nach Interpretation feinjustieren

	// Safety: Gegenteil von Angst plus etwas Trust
	const float SafetyRaw =
		0.5f * TrustDis
		- 0.8f * FMath::Max(0.0f, FearAnger); // Fear -> Safety-

	Affinity = FMath::Clamp(AffinityRaw, -1.0f, 1.0f);
	Trust    = FMath::Clamp(TrustRaw,    -1.0f, 1.0f);
	Respect  = FMath::Clamp(RespectRaw,  -1.0f, 1.0f);
	Safety   = FMath::Clamp(SafetyRaw,   -1.0f, 1.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
int32 GetMappedIndex(float Value, const int32 NumElements)
{
	Value = FMath::Clamp(Value, -1.0f, 1.0f);
	int32 Index = static_cast<int32>(((Value + 1.0f) * 0.5f) * NumElements);
	return Index >= NumElements ? NumElements-1 : Index;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void CollectValues(FString& Result, const float Value, const FString Descriptions[])
{
	const int32 Index = GetMappedIndex(Value, 5);
	if (Index != 2)
	{
		if (Result.Len() > 0)
		{
			Result.AppendChar(',');
		}
		Result.Append(Descriptions[Index]);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FString GetRelationShipDescription(const FRelationshipState& RelationshipState)
{
	static const FString Affinity[] = { TEXT("Hostile"), TEXT("Weary"), TEXT("Neutral"), TEXT("Friendly"), TEXT("Affectionate") };
	static const FString Trust[] = { TEXT("Distrustful"), TEXT("Skeptical"), TEXT("Neutral"), TEXT("Trusting"), TEXT("Confident") };
	static const FString Respect[] = { TEXT("Contemptuous"), TEXT("Dismissive"), TEXT("Neutral"), TEXT("Respectful"), TEXT("Reverent") };
	static const FString Safety[] = { TEXT("Threatened"), TEXT("Uneasy"), TEXT("Neutral"), TEXT("Secure"), TEXT("Protected") };
	
	FString Result;
	CollectValues(Result, RelationshipState.Affinity, Affinity);
	CollectValues(Result, RelationshipState.Trust, Trust);
	CollectValues(Result, RelationshipState.Respect, Respect);
	CollectValues(Result, RelationshipState.Safety, Safety);
	if (Result.IsEmpty())
	{
		Result = TEXT("Neutral");
	}
	return Result;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------
FString GetEmotionDescription(const EPrimaryEmotionAxis Axis, const EEmotionalLevel Level)
{
	static const FString JoySadness[] = { TEXT("Grieving"), TEXT("Sad"), TEXT("Pensive"), TEXT("Calm"), TEXT("Serene"), TEXT("Joyful"), TEXT("Ecstatic") };
	static const FString TrustDisgust[] = { TEXT("Loathing"), TEXT("Disgusted"), TEXT("Bored"), TEXT("Calm"), TEXT("Accepting"), TEXT("Trusting"), TEXT("Admiring") };
	static const FString FearAnger[] = { TEXT("Enraged"), TEXT("Angry"), TEXT("Annoyed"), TEXT("Calm"), TEXT("Apprehensive"), TEXT("Fearful"), TEXT("Terrified") };
	static const FString SurpriseAnticipation[] = { TEXT("Vigilant"), TEXT("Anticipating"), TEXT("Interested"), TEXT("Calm"), TEXT("Distracted"), TEXT("Surprised"), TEXT("Amazed") };
	
	const int32 Index = static_cast<int32>(Level);
	switch (Axis)
	{
	case EPrimaryEmotionAxis::JoySadness:
		return JoySadness[Index];
	case EPrimaryEmotionAxis::TrustDisgust:
		return TrustDisgust[Index];
	case EPrimaryEmotionAxis::FearAnger:
		return FearAnger[Index];
	case EPrimaryEmotionAxis::SurpriseAnticipation:
		return SurpriseAnticipation[Index];
	}
	static FString Undefined = TEXT("???");
	return Undefined;
}
