// (c) 2025 MK


#include "AI/Goals/GoalCollectStone.h"

#include "AI/IAgent.h"
#include "Resources/ResourceRegistrySubsystem.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
FString UGoalCollectStone::Describe(IAgent& Agent, const FWorldState& WorldState) const
{
	return TEXT("gathering stone.");
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const UResourceDefinition* UGoalCollectStone::GetResource(IAgent& Agent) const
{
	return Agent.GetResourceRegistry()->FindById("Stone");
}
