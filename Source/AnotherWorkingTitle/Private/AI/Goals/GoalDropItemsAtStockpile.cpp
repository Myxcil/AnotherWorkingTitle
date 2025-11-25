// (c) 2025 MK


#include "AI/Goals/GoalDropItemsAtStockpile.h"

#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "AI/Planning/WorldState.h"
#include "Inventory/Stockpile.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
float UGoalDropItemsAtStockpile::Evaluate(IAgent& Agent) const
{
	const int32 CarryingAmount = Agent.GetAmountInInventory(nullptr);
	if (CarryingAmount == 0)
		return 0;
	
	const AStockpile* NearestStockpile = FAIHelper::FindNearestStockpile(Agent.GetGroundPosition());
	if (!NearestStockpile)
		return 0;
	
	return 0.4f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGoalDropItemsAtStockpile::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	const int32 CarryingAmount = Agent.GetAmountInInventory(nullptr);
	if (CarryingAmount == 0)
		return false;
	
	AStockpile* NearestStockpile = FAIHelper::FindNearestStockpile(Agent.GetGroundPosition());
	if (!NearestStockpile)
		return false;
	
	WorldState.Set(EWorldPropertyKey::Interact, NearestStockpile);
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UGoalDropItemsAtStockpile::DeInit(IAgent& Agent, bool bIsSuccess) const
{
	
}
