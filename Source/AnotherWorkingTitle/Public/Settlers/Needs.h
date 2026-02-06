// (c) 2025 MK


#pragma once

#include "CoreMinimal.h"
#include "Needs.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ENeedSeverity : uint8
{
	Normal,
	Uncomfortable,
	Critical,
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ENeedType : uint8
{
	Unknown UMETA(Hidden),
	Hunger,
	Thirst,
	Cold,
	Fatigue,
	Damage,
};
ENUM_RANGE_BY_FIRST_AND_LAST(ENeedType, ENeedType::Hunger, ENeedType::Damage);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FNeeds
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Hunger = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Thirst = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cold = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Fatigue = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.0f;
};

