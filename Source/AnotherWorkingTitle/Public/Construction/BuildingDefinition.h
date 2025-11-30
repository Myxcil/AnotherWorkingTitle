// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingDefinition.generated.h"

class ABaseBuilding;
class UResourceDefinition;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FResourceAmount
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UResourceDefinition> Resource = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 0;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FBuildCost
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceAmount> Requirements;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UBuildingDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building")
	FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building")
	FBuildCost BuildCost;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building")
	float TimeToBuild = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building")
	TSubclassOf<ABaseBuilding> BuildingToSpawn;
};
