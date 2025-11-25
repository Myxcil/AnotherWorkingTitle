// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "ResourceCategory.h"
#include "Engine/DataAsset.h"
#include "ResourceDefinition.generated.h"

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
	EResourceCategory Category = EResourceCategory::Material; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	int32 UnitWeight = 0;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(AssetType, GetFName());
	}
};
