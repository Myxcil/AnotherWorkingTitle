// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactive/Interaction.h"
#include "BuildingSite.generated.h"

class AStockpile;
class UBuildingDefinition;
struct FResourceAmount;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuildingProgress, float, Progress);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API ABuildingSite : public AActor, public IHoldInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ABuildingSite();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;

public:
	//------------------------------------------------------------------------------------------------------------------------------------------------------------
	// IHoldInteraction
	virtual void BeginInteraction_Implementation(ASettlerCharacter* SettlerCharacter) override;
	virtual bool TickInteraction_Implementation(ASettlerCharacter* SettlerCharacter, float DeltaTime) override;
	virtual void EndInteraction_Implementation(ASettlerCharacter* SettlerCharacter) override;
	
	bool HasResourcesForBuild() const;
	bool ApplyWork(const float Amount);

	UPROPERTY(BlueprintAssignable)
	FBuildingProgress OnBuildingProgress;
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool TryConsumeBuildResources();
	bool CanAfford(const TArray<AStockpile*>& AllStockpiles, const TArray<FResourceAmount>& Requirements) const;
	void OnBuildCompleted();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBuildingDefinition> BuildingDefinition;
	
	bool bIsBuildStarted = false;
	float BuildProgress = 0;
};
