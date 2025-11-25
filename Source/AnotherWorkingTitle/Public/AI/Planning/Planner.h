#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------------------
#include "WorldState.h"

class IAgent;
class UAbstractAction;
class UAbstractGoal;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
struct FPlanStep
{
	const UAbstractAction* Action;
	FWorldState WorldState;

	void WriteToLog() const;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
class FPlanner
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FPlanner();
	~FPlanner();

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	bool FindValidPlan(IAgent& Agent, const UAbstractGoal* Goal, FWorldState& GoalState, float& Cost);
	bool GeneratePlan(IAgent& Agent, const UAbstractGoal* Goal, const FWorldState& GoalState, TArray<FPlanStep>& Plan) const;
	
private:
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	struct FNode
	{
		const UAbstractAction* Action;
		FNode* Parent;

		FWorldState CurrentState;
		FWorldState GoalState;

		int32 CostFromStart;
		int32 EstimatedCostToGoal;
		int32 TotalCost;

		bool operator<(const FNode& OtherNode) const { return TotalCost >= OtherNode.TotalCost; }
	};

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	FNode* Allocate();
	void Free(FNode* Node);

	static void Merge(const FWorldState& AgentState, const FWorldState& GoalState, FWorldState& CurrentState);
	void AddToOpenList(FNode* Node);
	FNode* PopFromOpenList();

	static bool IsPlanValid(IAgent* Agent, const FNode* Node);
	void UpdateNeighborActions(IAgent* Agent, const FNode* Node);

	static int32 CalculateCostFromStart(IAgent* Agent, const FNode* Node, FNode* NextNode);
	static int32 EstimateCostToGoal(IAgent* Agent, FNode* Node);

	static void CreatePlanFromNodes(IAgent& Agent, const FNode* Node, TArray<FPlanStep>& Plan);
	
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	TArray<FNode*> OpenList;
	TArray<FNode*> NodePool;
	TArray<const UAbstractAction*> NeighborActions;
	FNode* RootNode = nullptr;
	FNode* FinalNode = nullptr;
};
