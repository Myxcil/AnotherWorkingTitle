// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
#include "Construction/BaseBuilding.h"
#include "Settlers/Needs.h"
#include "NeedInteraction.generated.h"

class UGameTimeSubsystem;
class ASettlerCharacter;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class ANOTHERWORKINGTITLE_API ANeedInteraction : public ABaseBuilding, public IHoldInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static const TArray<ANeedInteraction*>& GetInstances();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ANeedInteraction();

protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	// IHoldInteraction
	virtual void BeginInteraction_Implementation(ASettlerCharacter* SettlerCharacter) override;
	virtual void EndInteraction_Implementation(ASettlerCharacter* SettlerCharacter) override;
	virtual bool TickInteraction_Implementation(ASettlerCharacter* SettlerCharacter, float DeltaTime) override;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ENeedType GetAffectedType() const { return AffectedType; }
	float GetNeedDelta() const { return NeedsValueDelta; }
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool StartInteraction(ASettlerCharacter* Settler);
	void StopInteraction();
	bool IsInteracting() const { return CurrentUser.IsValid();}
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool UpdateInteraction();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	ENeedType AffectedType = ENeedType::Fatigue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Needs")
	float NeedsValueDelta = 0.0f;
	
	TWeakObjectPtr<const UGameTimeSubsystem> GameTimeSubsystemPtr;
	TWeakObjectPtr<ASettlerCharacter> CurrentUser;
};
