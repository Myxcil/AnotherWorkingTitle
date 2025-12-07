// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "ResourceCategory.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ResourceRegistrySubsystem.generated.h"

class UResourceDefinition;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API UResourceRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	const UResourceDefinition* FindById(const FName Id) const;

	const TArray<const UResourceDefinition*>& GetAllResources() const { return Resources; }
	const TArray<const UResourceDefinition*>& GetResources(const EResourceCategory Category) const { return ResourceMapByCategory[Category]; }

private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY()
	TArray<const UResourceDefinition*> Resources;
	
	TMap<EResourceCategory, TArray<const UResourceDefinition*>> ResourceMapByCategory;
};
