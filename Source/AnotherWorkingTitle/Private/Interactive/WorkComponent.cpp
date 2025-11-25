// (c) 2025 MK


#include "Interactive/WorkComponent.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
UWorkComponent::UWorkComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UWorkComponent::StartWork(AActor* Target)
{
	if (CurrentWorkTarget.IsValid())
		return;
	
	if (!Target)
		return;
	
	CurrentWorkTarget = Target;
	
	SetComponentTickEnabled(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UWorkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!CurrentWorkTarget.IsValid())
	{
		StopWork();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UWorkComponent::StopWork()
{
	SetComponentTickEnabled(false);
	
	CurrentWorkTarget = nullptr;
}

