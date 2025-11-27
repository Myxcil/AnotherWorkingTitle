// (c) 2025 MK


#include "AI/Actions/ActionDepositResources.h"

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
	if (!PropDeposit || PropDeposit->Type != EWorldPropertyType::Object || !PropDeposit->Object)
		return false;
	
	const AStockpile* Stockpile = Cast<AStockpile>(PropDeposit->Object);
	if (!Stockpile)
		return false;
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionDepositResources::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropDeposit = CurrentWorldState.Get(EWorldPropertyKey::Transfer);
	check(PropDeposit);
	check(PropDeposit->Type == EWorldPropertyType::Object);
	check(PropDeposit->Object);
	
	AStockpile* Stockpile = Cast<AStockpile>(PropDeposit->Object);
	Agent.DepositAll(Stockpile);
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionDepositResources::Deactivate(IAgent& Agent, FAIState& AIState) const
{

}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionDepositResources::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return EActionResult::Complete;
}
