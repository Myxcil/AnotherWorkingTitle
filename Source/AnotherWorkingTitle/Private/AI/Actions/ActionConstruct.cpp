// (c) 2025 MK


#include "AI/Actions/ActionConstruct.h"

#include "AI/IAgent.h"
#include "Construction/BuildingSite.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
UActionConstruct::UActionConstruct()
{
	Preconditions.Set(EWorldPropertyKey::AtNode, EWorldPropertyKey::Construct);
	Results.Set(EWorldPropertyKey::Construct, EWorldPropertyKey::Construct);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionConstruct::AreContextPreconditionsSatisfied(IAgent& Agent, const FWorldState& CurrentWorldState, const bool bIsPlanning) const
{
	if (!Super::AreContextPreconditionsSatisfied(Agent, CurrentWorldState, bIsPlanning))
		return false;
	
	const FWorldProperty* PropConstruct = CurrentWorldState.Get(EWorldPropertyKey::Construct);
	if (!PropConstruct || PropConstruct->Type != EWorldPropertyType::Object || !PropConstruct->Object)
		return false;
	
	const ABuildingSite* BuildingSite = Cast<ABuildingSite>(PropConstruct->Object);
	if (!BuildingSite)
		return false;
	
	if (!bIsPlanning)
	{
		if (!BuildingSite->CanConstruct())
			return false;
	}
	
	return true;

}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionConstruct::Activate(IAgent& Agent, FAIState& AIState, const FWorldState& CurrentWorldState) const
{
	const FWorldProperty* PropInteract = CurrentWorldState.Get(EWorldPropertyKey::Construct);
	check(PropInteract);
	check(PropInteract->Type == EWorldPropertyType::Object);
	check(PropInteract->Object);
	
	ABuildingSite* BuildingSite = Cast<ABuildingSite>(PropInteract->Object);
	BuildingSite->ApplyWork(0.1f);
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionConstruct::Deactivate(IAgent& Agent, FAIState& AIState) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionConstruct::IsComplete(IAgent& Agent, FAIState& AIState) const
{
	return EActionResult::Complete;
}
