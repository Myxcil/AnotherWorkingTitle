// (c) 2025 MK


#include "Interactive/NeedsModifier.h"

#include "GameTimeSubsystem.h"
#include "AI/AIHelper.h"
#include "Components/SphereComponent.h"
#include "Settlers/NeedsComponent.h"
#include "Settlers/SettlerCharacter.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ANeedsModifier::ANeedsModifier()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>("InfluenceSphere");
	SphereComponent->SetSphereRadius(100.0f);
	SetRootComponent(SphereComponent);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedsModifier::BeginPlay()
{
	Super::BeginPlay();
	
	GameTimeSubsystemPtr = GetWorld()->GetSubsystem<UGameTimeSubsystem>();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedsModifier::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (int32 I=0; I < OverlappingNeedsComponents.Num(); ++I)
	{
		if (UNeedsComponent* NeedsComponent = OverlappingNeedsComponents[I].Get())
		{
			NeedsComponent->OnNeedsModifierChanged.Broadcast(this, false);
		}
	}
	OverlappingNeedsComponents.Reset();
	
	if (SphereComponent)
	{
		SphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnComponentBeginOverlap);
		SphereComponent->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnComponentEndOverlap);
	}
	
	Super::EndPlay(EndPlayReason);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedsModifier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const UGameTimeSubsystem* GameTimeSubsystem = GameTimeSubsystemPtr.Get();
	if (!GameTimeSubsystem)
		return;
	
	const float Radius = FMath::Max(1.0f, SphereComponent->GetScaledSphereRadius());
	const float DeltaValue = NeedsValueDelta * GameTimeSubsystem->GetGameDeltaHour();
	for (int32 I=OverlappingNeedsComponents.Num()-1; I >= 0; --I)
	{
		if (UNeedsComponent* NeedsComponent = OverlappingNeedsComponents[I].Get())
		{
			const float EffectFalloff = bFallOff ? 1.0f - FAIHelper::Clamp01(GetDistanceTo(NeedsComponent->GetOwner()) / Radius) : 1.0f;
			NeedsComponent->ChangeNeedValue(AffectedType, DeltaValue * EffectFalloff);
		}
		else
		{
			OverlappingNeedsComponents.RemoveAt(I);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedsModifier::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASettlerCharacter* SettlerCharacter = Cast<ASettlerCharacter>(OtherActor))
	{
		if (UNeedsComponent* NeedsComponent = SettlerCharacter->GetNeedsComponent())
		{
			if (!OverlappingNeedsComponents.Contains(NeedsComponent))
			{
				NeedsComponent->OnNeedsModifierChanged.Broadcast(this, true);
				OverlappingNeedsComponents.Add(NeedsComponent);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void ANeedsModifier::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const int32 Index = OverlappingNeedsComponents.IndexOfByPredicate([OtherActor](const TWeakObjectPtr<UNeedsComponent>& NeedsComponentPtr)
	{
		if (const UNeedsComponent* NeedsComponent = NeedsComponentPtr.Get())
		{
			return NeedsComponent->GetOwner() == OtherActor;
		}
		return false;
	});
	if (Index != INDEX_NONE)
	{
		if (const UNeedsComponent* NeedsComponent = OverlappingNeedsComponents[Index].Get())
		{
			NeedsComponent->OnNeedsModifierChanged.Broadcast(this, false);
		}
		OverlappingNeedsComponents.RemoveAtSwap(Index);
	}
}

