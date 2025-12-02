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

	// Beispiel-Gewichtungen – reine Startwerte, später feintunen:

	// Mögen: viel Joy und Trust, wenig Disgust/Anger
	const float AffinityRaw =
		0.6f * JoySad +        // Joy -> Affinity+
		0.4f * TrustDis       // Trust -> Affinity+
		- 0.3f * FMath::Min(0.0f, TrustDis)  // Disgust (negativ) -> Affinity-
		- 0.3f * FMath::Min(0.0f, FearAnger);// Anger (negativ) -> Affinity-

	// Vertrauen: stark von Trust/Disgust geprägt, Angst reduziert es
	const float TrustRaw =
		0.9f * TrustDis
		- 0.3f * FMath::Max(0.0f, FearAnger); // Fear -> Trust-

	// Respekt: Mischung aus wahrgenommener Kompetenz/Überraschung
	// hier grob mit Surprise/Anticipation gekoppelt (jemand, der oft positiv "überrascht"/vorausschauend ist)
	float respectRaw =
		0.3f * TrustDis +
		0.4f * SurprAnt; // +Surprise / -Anticipation kannst du je nach Interpretation feinjustieren

	// Safety: Gegenteil von Angst plus etwas Trust
	const float SafetyRaw =
		0.5f * TrustDis
		- 0.8f * FMath::Max(0.0f, FearAnger); // Fear -> Safety-

	Affinity = FMath::Clamp(AffinityRaw, -1.0f, 1.0f);
	Trust    = FMath::Clamp(TrustRaw,    -1.0f, 1.0f);
	Respect  = FMath::Clamp(respectRaw,  -1.0f, 1.0f);
	Safety   = FMath::Clamp(SafetyRaw,   -1.0f, 1.0f);
}
