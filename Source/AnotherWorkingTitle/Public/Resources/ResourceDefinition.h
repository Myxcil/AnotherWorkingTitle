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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource", meta=(ClampMin="1"))
	int32 MaxStackSize = 10; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resource")
	TObjectPtr<UTexture2D> Icon = nullptr;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(AssetType, GetFName());
	}
};
