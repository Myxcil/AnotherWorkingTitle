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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuildCostUpdated, const TArray<FResourceAmount>&, Missing);

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API ABuildingSite : public AActor, public IHoldInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static const TArray<ABuildingSite*>& GetInstances();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ABuildingSite();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//------------------------------------------------------------------------------------------------------------------------------------------------------------
	// IHoldInteraction
	virtual void BeginInteraction_Implementation(ASettlerCharacter* SettlerCharacter) override;
	virtual bool TickInteraction_Implementation(ASettlerCharacter* SettlerCharacter, float DeltaTime) override;
	virtual void EndInteraction_Implementation(ASettlerCharacter* SettlerCharacter) override;

	float GetCurrentProgress() const { return BuildProgress; }
	bool CanConstruct() const;
	
	bool HasResourcesForBuild() const;
	bool ApplyWork(const float Amount);

	UPROPERTY(BlueprintAssignable)
	FBuildingProgress OnBuildingProgress;
	UPROPERTY(BlueprintAssignable)
	FBuildCostUpdated OnBuildCostUpdated; 
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool TryConsumeBuildResources();
	bool CanAfford(const TArray<AStockpile*>& AllStockpiles, const TArray<FResourceAmount>& Requirements) const;
	void OnBuildCompleted();
	UFUNCTION()
	void OnStockpileUpdated();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<const UBuildingDefinition> BuildingDefinition;
	
	bool bIsBuildStarted = false;
	float BuildProgress = 0;
};
