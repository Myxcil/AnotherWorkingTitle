// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "ResourceCategory.h"
#include "Engine/DataAsset.h"
#include "Settlers/Needs.h"
#include "ResourceDefinition.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
enum { InvalidRuntimeId = 0 };

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UResourceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const FPrimaryAssetType AssetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	FName Id;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource", meta=(ClampMin="1"))
	int32 MaxStackSize = 10; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	float BaseValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	EResourceCategory Category = EResourceCategory::RawMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	float HungerChange = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	float ThirstChange = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	float ColdChange = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	float FatigueChange = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	float DamageChange = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	float UseTimeInSeconds = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	TSubclassOf<AActor> WorldPickupClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	TSubclassOf<AActor> PlaceableActorClass;

	UPROPERTY(Transient)
	int32 RuntimeId = InvalidRuntimeId;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(AssetType, GetFName());
	}

	float GetNeedChange(ENeedType NeedType) const;
};
