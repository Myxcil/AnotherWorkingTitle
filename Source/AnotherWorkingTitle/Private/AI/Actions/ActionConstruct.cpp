// (c) 2025 MK


#include "AI/Actions/ActionConstruct.h"

#include "AI/AIBlackboard.h"
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
	if (!PropConstruct || PropConstruct->Type != EWorldPropertyType::Node || PropConstruct->NodeType != ENodeType::BuildingSite)
		return false;
	
	if (!bIsPlanning)
	{
		const ABuildingSite* BuildingSite = Agent.GetBlackboard().GetBuildingSite();
		if (!BuildingSite)
			return false;
				
		if (!BuildingSite->CanConstruct())
			return false;
	}
	
	return true;

}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
bool UActionConstruct::Activate(IAgent& Agent, const FWorldState& CurrentWorldState) const
{
	ABuildingSite* BuildingSite = Agent.GetBlackboard().GetBuildingSite();
	BuildingSite->ApplyWork(0.1f);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
void UActionConstruct::Deactivate(IAgent& Agent) const
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
EActionResult UActionConstruct::IsComplete(IAgent& Agent) const
{
	return EActionResult::Complete;
}
