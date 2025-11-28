// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Settlers/Needs.h"
#include "NeedsModifier.generated.h"

class UGameTimeSubsystem;
class UNeedsComponent;
class USphereComponent;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(BlueprintType)
class ANOTHERWORKINGTITLE_API ANeedsModifier : public AActor
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ANeedsModifier();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Tick(float DeltaTime) override;
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	ENeedType AffectedType = ENeedType::Cold;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	float NeedsValueDelta = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	bool bFallOff = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	TObjectPtr<UTexture2D> EffectIcon = nullptr;
		
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	TWeakObjectPtr<const UGameTimeSubsystem> GameTimeSubsystemPtr;
	TArray<TWeakObjectPtr<UNeedsComponent>> OverlappingNeedsComponents;
};
