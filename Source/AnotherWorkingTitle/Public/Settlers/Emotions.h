// (c) 2025 MK


#pragma once

#include "CoreMinimal.h"
#include "Emotions.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EPrimaryEmotionAxis : uint8
{
	JoySadness,
	TrustDisgust,
	FearAnger,
	SurpriseAnticipation,
};
ENUM_RANGE_BY_FIRST_AND_LAST(EPrimaryEmotionAxis, EPrimaryEmotionAxis::JoySadness, EPrimaryEmotionAxis::SurpriseAnticipation);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FEmotionalState
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float JoySadness = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float TrustDisgust = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float FearAnger = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float SurpriseAnticipation = 0;
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------
	void SetValue(const EPrimaryEmotionAxis Axis, const float NewValue)
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
	float GetValue(const EPrimaryEmotionAxis Axis) const
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
	bool ChangeValue(const EPrimaryEmotionAxis Axis, const float Delta)
	{
		const float OldValue = GetValue(Axis);
		SetValue(Axis, OldValue + Delta);
		return OldValue != GetValue(Axis);
	}
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EEmotion : uint8
{
	// Joy intensity
	Calm,
	Pleased,
	Delighted,
	
	// Trust intensity
	Open,
	Safe,
	Proud,
	
	// Fear intensity
	Worried,
	Scared,
	Alarmed,
	
	// Surprise intensity
	Uncertain,
	Shocked,
	Amazed,
	
	// Disgust intensity
	Bored,
	Disgusted,
	Loathing,
	
	// Anger intensity
	Annoyed,
	Mad,
	Furious,
	
	// Anticipation intensity
	Interested,
	Curious,
	Focused,
	
	// Intermediate
	Connected,		// Joy + Trust
	Obedient,		// Trust + Fear
	Horrified,		// Fear + Sadness
	Remorseful,		// Sadness + Anger
	Righteous,		// Anger + Disgust
	Hostile,		// Disgust + Anticipation
	Hopeful,		// Anticipation + Joy
	
	Undecided,
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FEmotionAxisTemperament
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(CLampMin="-1.0", ClampMax="1.0"))
	float Bias = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(CLampMin="0.0", ClampMax="1.0"))
	float Reactivity = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(CLampMin="0.0", ClampMax="1.0"))
	float Regulation = 0.5f;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FEmotionTemperament
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionAxisTemperament JoySadness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionAxisTemperament TrustDisgust;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionAxisTemperament FearAnger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionAxisTemperament SurpriseAnticipation;

	FEmotionAxisTemperament& GetAxis(const EPrimaryEmotionAxis Axis)
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
		default:
			return SurpriseAnticipation;
		}
	}

	const FEmotionAxisTemperament& GetAxis(const EPrimaryEmotionAxis Axis) const
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
		default:
			return SurpriseAnticipation;
		}
	}
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(BlueprintType)
class UEmotionalArchetype : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ArchetypeId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEmotionTemperament Temperament;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEmotionalState BaselineEmotion;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="2.0"))
	float GlobalIntensity = 1.0f;
};
