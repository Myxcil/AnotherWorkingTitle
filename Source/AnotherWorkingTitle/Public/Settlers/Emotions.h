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
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float JoySadness = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float TrustDisgust = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float FearAnger = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float SurpriseAnticipation = 0;
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------
	void SetValue(const EPrimaryEmotionAxis Axis, const float NewValue);
	float GetValue(const EPrimaryEmotionAxis Axis) const;
	bool ChangeValue(const EPrimaryEmotionAxis Axis, const float Delta);
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EEmotionalLevel : uint8
{
	Worst,
	Worse,
	Bad,
	Neutral,
	Good,
	Better,
	Best,
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FEmotionalThreshold
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", ClampMax="1.0"))
	float Low = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", ClampMax="1.0"))
	float Med = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", ClampMax="1.0"))
	float High = 0.8f;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FEmotionalThresholds
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalThreshold Joy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalThreshold Sadness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalThreshold Trust;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalThreshold Disgust;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalThreshold Fear;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalThreshold Anger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalThreshold Surprise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalThreshold Anticipation;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FEmotionSummary
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEmotionalLevel JoySadness = EEmotionalLevel::Neutral;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEmotionalLevel TrustDisgust = EEmotionalLevel::Neutral;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEmotionalLevel FearAnger = EEmotionalLevel::Neutral;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEmotionalLevel SurpriseAnticipation = EEmotionalLevel::Neutral;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPrimaryEmotionAxis MaxEmotion;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEmotionalLevel MaxEmotionLevel = EEmotionalLevel::Neutral;
	
	void Evaluate(const FEmotionalState& EmotionalState, const FEmotionalThresholds& EmotionalThresholds);
	bool operator==(const FEmotionSummary& Other) const = default;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FEmotionAxisTemperament
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Bias = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", ClampMax="1.0"))
	float Reactivity = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", ClampMax="1.0"))
	float Regulation = 0.5f;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FEmotionTemperament
{
	GENERATED_BODY()
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionAxisTemperament JoySadness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionAxisTemperament TrustDisgust;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionAxisTemperament FearAnger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionAxisTemperament SurpriseAnticipation;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
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

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
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
UENUM(BlueprintType)
enum class ERelationShipAspect : uint8
{
	Affinity,
	Trust,
	Respect,
	Safety,
};
ENUM_RANGE_BY_FIRST_AND_LAST(ERelationShipAspect, ERelationShipAspect::Affinity, ERelationShipAspect::Safety);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FRelationshipState
{
	GENERATED_BODY()

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalState LongTermEmotion;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Affinity = 0.0f;  // mögen / Sympathie
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Trust = 0.0f;     // Vertrauen / Misstrauen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Respect = 0.0f;   // Respekt / Geringschätzung
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Safety = 0.0f;    // Sicherheitsgefühl / Bedrohung

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERelationShipAspect MaxAspect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxValue = 0.0f;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void ApplyEmotionDelta(const FEmotionalState& Delta, float Weight = 1.0f, float Smoothing = 0.2f); 
	void RecomputeDerivedValues();
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(BlueprintType)
class UEmotionalArchetype : public UDataAsset
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ArchetypeId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEmotionTemperament Temperament;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEmotionalThresholds Thresholds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEmotionalState BaselineEmotion;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="2.0"))
	float GlobalIntensity = 1.0f;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FString GetRelationShipDescription(const ERelationShipAspect Aspect, const float Value);
FString GetEmotionDescription(const EPrimaryEmotionAxis Axis, const EEmotionalLevel Level);