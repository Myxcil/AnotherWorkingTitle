// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
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

private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY()
	TArray<UResourceDefinition*> Resources;
};
