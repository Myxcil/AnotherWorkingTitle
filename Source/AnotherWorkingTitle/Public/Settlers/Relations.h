// (c) 2025 MK


#pragma once

#include "CoreMinimal.h"
#include "Emotions.h"
#include "Relations.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FRelationshipState
{
	GENERATED_BODY()

	/** Langfristige, geglättete Emotionen, die Self gegenüber Other empfindet. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmotionalState LongTermEmotion;

	/** Abgeleitete, kompakte Relationship-Dimensionen [-1..+1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Affinity = 0.0f;  // mögen / Sympathie
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Trust = 0.0f;     // Vertrauen / Misstrauen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Respect = 0.0f;   // Respekt / Geringschätzung
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="-1.0", ClampMax="1.0"))
	float Safety = 0.0f;    // Sicherheitsgefühl / Bedrohung

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void ApplyEmotionDelta(const FEmotionalState& Delta, float Weight = 1.0f, float Smoothing = 0.2f); 
	void RecomputeDerivedValues();
};
