// (c) 2024 by Crenetic GmbH Studios

//------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "AI/AIHelper.h"

#include "AI/AIConstants.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FAIHelper::GetObjectTransform(const UObject* Object, FTransform& Result)
{
	check(Object);
	
	if (const AActor* Actor = Cast<AActor>(Object))
	{
		Result = Actor->GetActorTransform();
		return true;
	}

	if (const USceneComponent* SceneComponent = Cast<USceneComponent>(Object))
	{
		Result = SceneComponent->GetComponentTransform();
		return true;
	}

	AI_WARN(TEXT("GetObjectTransform, unhandled object %s (%s)"), *Object->GetName(), *Object->GetClass()->GetName());
	Result = FTransform::Identity;
	return false;
}
