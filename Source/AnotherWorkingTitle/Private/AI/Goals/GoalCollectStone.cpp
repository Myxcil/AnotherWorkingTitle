// (c) 2025 MK


#include "AI/Goals/GoalCollectStone.h"

#include "AI/IAgent.h"
#include "Resources/ResourceRegistrySubsystem.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
const UResourceDefinition* UGoalCollectStone::GetResource(IAgent& Agent) const
{
	return Agent.GetResourceRegistry()->FindById("Stone");
}
