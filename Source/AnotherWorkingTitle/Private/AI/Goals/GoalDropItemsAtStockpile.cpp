// (c) 2025 MK


#include "AI/Goals/GoalDropItemsAtStockpile.h"

#include "AI/AIGoalMapping.h"
#include "AI/AIHelper.h"
#include "AI/IAgent.h"
#include "AI/Planning/WorldState.h"
#include "Inventory/Stockpile.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
float UGoalDropItemsAtStockpile::Evaluate(IAgent& Agent) const
{
	if (Agent.IsInCriticalState())
		return 0;
	
	const int32 CarryingAmount = Agent.GetTotalAmountInInventoryByCategory(EResourceCategory::RawMaterial);
	if (CarryingAmount == 0)
		return 0;
	
	const AStockpile* NearestStockpile = FAIHelper::FindNearestStockpile(Agent.GetFeetPosition());
	if (!NearestStockpile)
		return 0;
	
	const float CarryingWeight = FAIHelper::Clamp01(0.05f * CarryingAmount);
	const float DistanceWeight = FAIHelper::CalculateDistanceWeight(Agent.GetFeetPosition(), NearestStockpile->GetActorLocation());
	const float NormalizedPriority = CarryingWeight * DistanceWeight;

	return FMath::GetRangeValue(GoalPriorities::PriorityScaleDeposit, NormalizedPriority);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool UGoalDropItemsAtStockpile::Init(IAgent& Agent, FWorldState& WorldState, bool bIsPlanning) const
{
	AStockpile* NearestStockpile = FAIHelper::FindNearestStockpile(Agent.GetFeetPosition());
	if (!NearestStockpile)
		return false;
	
	WorldState.Set(EWorldPropertyKey::Transfer, NearestStockpile);
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void UGoalDropItemsAtStockpile::DeInit(IAgent& Agent, bool bIsSuccess) const
{
	
}
