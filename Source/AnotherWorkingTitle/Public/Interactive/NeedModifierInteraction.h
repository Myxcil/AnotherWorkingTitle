// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "AbstractNeedModifier.h"
#include "Interaction.h"
#include "NeedModifierInteraction.generated.h"

class ASettlerCharacter;

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UCLASS(Abstract)
class ANOTHERWORKINGTITLE_API ANeedModifierInteraction : public AAbstractNeedModifier, public IHoldInteraction
{
	GENERATED_BODY()

public:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static const TArray<ANeedModifierInteraction*>& GetInstances();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	ANeedModifierInteraction();

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
	float StartInteraction(ASettlerCharacter* Settler);
	void StopInteraction();
	bool IsInteracting() const { return CurrentUser.IsValid();}
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	bool UpdateInteraction();
	
	TWeakObjectPtr<ASettlerCharacter> CurrentUser;
};
