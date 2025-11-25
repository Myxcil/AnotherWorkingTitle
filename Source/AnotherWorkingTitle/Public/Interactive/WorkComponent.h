// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorkComponent.generated.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API UWorkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UWorkComponent();

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	void StartWork(AActor* Target);
	void StopWork();
	bool IsWorking() const { return CurrentWorkTarget.IsValid(); }
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TWeakObjectPtr<AActor> CurrentWorkTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float WorkRate = 10.0f;
};
