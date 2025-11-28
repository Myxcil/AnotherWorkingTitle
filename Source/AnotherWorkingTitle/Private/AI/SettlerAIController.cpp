// (c) 2025 MK


#include "AI/SettlerAIController.h"

#include "NavigationSystem.h"
#include "AI/GOAPAgentComponent.h"
#include "Settlers/SettlerCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ASettlerAIController::ASettlerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ASettlerCharacter* Settler = Cast<ASettlerCharacter>(InPawn);
	if (!Settler)
	{
		CachedGOAP.Reset();
		return;
	}
	CachedSettler = Settler;
		
	if (UGOAPAgentComponent* GOAP = Settler->GetGOAPAgentComponent())
	{
		GOAP->OnAIRequestMoveToLocation.BindUObject(this, &ThisClass::HandleAIRequestMoveToLocation);
		GOAP->OnAIRequestStopMovement.BindUObject(this, &ThisClass::HandleAIRequestStopMovement);
		
		GOAP->InitializeAgent();
		
		CachedGOAP = GOAP;
	}
	
	if (PerceptionComponent)
	{
		PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ThisClass::OnPerceptionUpdate);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerAIController::OnUnPossess()
{
	if (UGOAPAgentComponent* GOAP = CachedGOAP.Get())
	{
		GOAP->OnAIRequestMoveToLocation.Unbind();
		GOAP->OnAIRequestStopMovement.Unbind();
	}
	
	CachedSettler.Reset();
	CachedGOAP.Reset();
	
	if (PerceptionComponent)
	{
		PerceptionComponent->OnPerceptionUpdated.RemoveDynamic(this, &ThisClass::OnPerceptionUpdate);
	}
	
	Super::OnUnPossess();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (const ASettlerCharacter* Settler = CachedSettler.Get())
	{
		if (Settler->IsDead())
			return;
	}
	
	if (UGOAPAgentComponent* GOAP = CachedGOAP.Get())
	{
		GOAP->TickGOAP(DeltaTime);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ASettlerAIController::HandleAIRequestMoveToLocation(const FVector& Location, const float LocationThreshold, const float AcceptanceRadius)
{
	const ASettlerCharacter* Settler = CachedSettler.Get();
	if (!Settler)
		return false;
	
	UGOAPAgentComponent* GOAP = CachedGOAP.Get();
	if (!GOAP)
		return false;
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
		return false;

	FNavLocation ProjectedLocation;
	const FVector QueryExtent(LocationThreshold, LocationThreshold, 200.f); // an dein Setup anpassen

	const bool bFoundNavPos = NavSys->ProjectPointToNavigation(Location, ProjectedLocation, QueryExtent, &Settler->GetNavAgentPropertiesRef());
	if (!bFoundNavPos)
	{
		// Ziel ist komplett außerhalb jedes NavMesh – MoveRequest nicht ausführen,
		// GOAP informieren, damit die aktuelle Action/Goal scheitern kann.
		AI_WARN(TEXT("%s failed to sample position at %s"), *Settler->GetName(), *Location.ToString());
		GOAP->OnMovementComplete(false);
		return false;
	}
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(ProjectedLocation.Location);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	MoveRequest.SetUsePathfinding(true);
	
	const EPathFollowingRequestResult::Type Result = MoveTo(MoveRequest); 
	if (Result == EPathFollowingRequestResult::Failed)
	{
		AI_WARN(TEXT("%s failed to move to %s, result=%d"), *Settler->GetName(), *ProjectedLocation.Location.ToString(), Result);
		return false;
	}

	if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		AI_WARN(TEXT("%s already at %s, diff=%f"), *Settler->GetName(), *ProjectedLocation.Location.ToString(), FVector::Distance(Settler->GetFeetPosition(), ProjectedLocation.Location));;
		return true;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerAIController::HandleAIRequestStopMovement()
{
	StopMovement();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	if (UGOAPAgentComponent* GOAP = CachedGOAP.Get())
	{
		const bool bSuccess = Result.IsSuccess();
		GOAP->OnMovementComplete(bSuccess);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ASettlerAIController::OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
{
}

