// (c) 2025 MK


#include "AI/Goals/GoalCollectWood.h"

#include "AI/IAgent.h"
#include "Resources/ResourceRegistrySubsystem.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FString UGoalCollectWood::Describe(IAgent& Agent, const FWorldState& WorldState) const
{
	return TEXT("harvesting wood.");
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const UResourceDefinition* UGoalCollectWood::GetResource(IAgent& Agent) const
{
	return Agent.GetResourceRegistry()->FindById("Wood");
}
