// (c) 2025 MK


#include "AI/Actions/ActionDepositResources.h"

#include "AI/AIBlackboard.h"
#include "AI/IAgent.h"
#include "Inventory/Stockpile.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionDepositResources::UActionDepositResources()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, EWorldPropertyKey::Transfer);
	Results.Set(EWorldPropertyKey::Transfer, EWorldPropertyKey::Transfer);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionDepositResources::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropDeposit = CurrentWorldState.Get(EWorldPropertyKey::Transfer);
	if (!PropDeposit || PropDeposit->Type != EWorldPropertyType::Node || PropDeposit->NodeType != ENodeType::Stockpile)
		return false;

	if (!bIsPlanning)
	{
		const AStockpile* Stockpile = Agent.GetBlackboard().GetStockpile();
		if (!Stockpile)
		{
			AI_WARN(TEXT("%s no stockpile registered"), *Agent.GetActorName());
			return false;
		}
	}
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionDepositResources::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	AStockpile* Stockpile = Agent.GetBlackboard().GetStockpile();
	Agent.DepositByCategory(Stockpile, EResourceCategory::RawMaterial);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionDepositResources::Deactivate(IAgent& Agent) const
{

}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionDepositResources::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
