// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SettlerAIController.generated.h"

class ASettlerCharacter;
class UGOAPAgentComponent;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS()
class ANOTHERWORKINGTITLE_API ASettlerAIController : public AAIController
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ASettlerAIController();

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool HandleAIRequestMoveToLocation(const FVector& Location, const float LocationThreshold, const float AcceptanceRadius);
	void HandleAIRequestPauseMovement(bool bSetPaused);
	void HandleAIRequestStopMovement();
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UFUNCTION()
	void OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors);
	
	TWeakObjectPtr<ASettlerCharacter> CachedSettler;
	TWeakObjectPtr<UGOAPAgentComponent> CachedGOAP;
	
	FAIRequestID CurrentMoveRequestID;
};
