// (c) 2025 MK


#include "AI/Goals/GoalCollectWood.h"

#include "AI/IAgent.h"
#include "Resources/ResourceRegistrySubsystem.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const UResourceDefinition* UGoalCollectWood::GetResource(IAgent& Agent) const
{
	return Agent.GetResourceRegistry()->FindById("Wood");
}
