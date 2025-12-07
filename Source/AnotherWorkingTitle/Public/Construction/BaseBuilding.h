// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBuilding.generated.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class ANOTHERWORKINGTITLE_API ABaseBuilding : public AActor
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	const TArray<ABaseBuilding*>& GetInstances();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ABaseBuilding();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
