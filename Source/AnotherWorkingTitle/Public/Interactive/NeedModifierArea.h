// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractNeedModifier.h"
#include "NeedModifierArea.generated.h"

class UNeedsComponent;
class USphereComponent;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class ANOTHERWORKINGTITLE_API ANeedModifierArea : public AAbstractNeedModifier
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static const TArray<ANeedModifierArea*>& GetInstances();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ANeedModifierArea();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Tick(float DeltaTime) override;
	
	float GetRadius() const;
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs|Area")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs|Area")
	bool bFallOff = true;
		
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	TArray<TWeakObjectPtr<UNeedsComponent>> OverlappingNeedsComponents;
};
