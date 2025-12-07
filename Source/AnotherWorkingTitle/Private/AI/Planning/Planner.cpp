// (c) 2024 by Crenetic GmbH Studios

#include "AI/Planning/Planner.h"
#include "AI/IAgent.h"
#include "AI/Goals/AbstractGoal.h"
#include "AI/Actions/AbstractAction.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------
void FPlanStep::WriteToLog() const
{
	if (Action)
	{
		AI_LOG(TEXT("%s"), *Action->GetTypeName());
		WorldState.WriteToLog();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
FPlanner::FPlanner()
{
	int32 NumPreallocNodes = 256;
	NodePool.Reserve(NumPreallocNodes);
	while (NumPreallocNodes--)
	{
		Free(new FNode());
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
FPlanner::~FPlanner()
{
	for (int32 Index = NodePool.Num() - 1; Index >= 0; --Index)
	{
		delete NodePool[Index];
	}
	NodePool.Empty();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
bool FPlanner::FindValidPlan(IAgent& Agent, const UAbstractGoal* Goal, FWorldState& GoalState, float& Cost)
{
	AI_LOG(TEXT("%s creating plan for %s"), *Agent.GetActorName(), *Goal->GetTypeName());

	// cleaning up
	FinalNode = nullptr;
	RootNode = nullptr;
	for (int32 Index = 0; Index < OpenList.Num(); ++Index)
	{
		Free(OpenList[Index]);
	}
	OpenList.Empty();

	GoalState.Clear();
	if (!Goal->Init(Agent, GoalState, true))
	{
		AI_WARN(TEXT("%s failed to init %s"), *Agent.GetActorName(), *Goal->GetTypeName());
		return false;
	}

	// create goal node
	RootNode = Allocate();
	RootNode->GoalState = GoalState;
	Merge(Agent.GetWorldState(), RootNode->GoalState, RootNode->CurrentState);
	RootNode->EstimatedCostToGoal = RootNode->CurrentState.CountDifferences(RootNode->GoalState);

	AddToOpenList(RootNode);

	// do the search
	int32 NumIterations = 0;
	for (;;)
	{
		FNode* Node = PopFromOpenList();
		if (Node == nullptr)
		{
			AI_WARN(TEXT("%s openlist empty planning %s, numIterations=%d"), *Agent.GetActorName(), *Goal->GetTypeName(), NumIterations);
			break;
		}

		if (IsPlanValid(&Agent, Node))
		{
			AI_LOG(TEXT("%s plan for %s, numIterations=%d"), *Agent.GetActorName(), *Goal->GetTypeName(), NumIterations);
			FinalNode = Node;

			const FNode* Iter = FinalNode;
			Cost = 0;
			FVector PlannedLocation = Agent.GetFeetPosition();
			while (Iter->Action)
			{
				const FNode* Parent = Iter->Parent;
				Cost += Iter->Action->CalculateRuntimeCost(Agent, Parent->GoalState, PlannedLocation);
				Iter = Parent;
			}
			break;
		}

		UpdateNeighborActions(&Agent, Node);

		for (const UAbstractAction* NextAction : NeighborActions)
		{
			if (NextAction == Node->Action)
				continue;

			FNode* NextNode = Allocate();
			NextNode->Action = NextAction;
			NextNode->Parent = Node;

			NextNode->CostFromStart = CalculateCostFromStart(&Agent, Node, NextNode);
			NextNode->EstimatedCostToGoal = EstimateCostToGoal(&Agent, NextNode);
			NextNode->TotalCost = NextNode->CostFromStart + NextNode->EstimatedCostToGoal;

			AddToOpenList(NextNode);
		}

		++NumIterations;
		if (NumIterations > 10000)
		{
			AI_WARN(TEXT("%s too many iterations on this plan %s"), *Agent.GetActorName(), *Goal->GetTypeName());
			break;
		}
	}

	return FinalNode != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
bool FPlanner::GeneratePlan(IAgent& Agent, const UAbstractGoal* Goal, const FWorldState& GoalState, TArray<FPlanStep>& Plan) const
{
	check(FinalNode);
	check(RootNode);

	// validate worldstates
	FWorldState TmpCurrState;
	FWorldState TmpGoalState(GoalState);
	Merge(Agent.GetWorldState(), TmpCurrState, TmpGoalState);

	const uint32 GoalMatchFlags = Goal->GetMatchFlags();
	if (GoalMatchFlags != 0 && !RootNode->GoalState.Matches(TmpGoalState, GoalMatchFlags))
	{
		FNode* Iter = FinalNode;
		while (Iter->Action)
		{
			FNode* Parent = Iter->Parent;
			const UAbstractAction* Action = Iter->Action;
		
			const FWorldState& ParentState = Parent->GoalState;
			if (!Action->AreContextPreconditionsSatisfied(Agent, ParentState, true))
			{
				AI_WARN(TEXT("GeneratePlan: %s, %s failed preconditions for %s"), *Agent.GetActorName(), *Goal->GetTypeName(), *Action->GetTypeName());
				return false;
			}

			Iter->CurrentState = TmpCurrState;
			Iter->GoalState = TmpGoalState;

			Action->SolveWorldState(Iter->CurrentState, Iter->GoalState);
			Action->ApplyPreconditions(Agent, Iter->GoalState);
			Merge(Agent.GetWorldState(), Iter->CurrentState, Iter->GoalState);

			TmpCurrState = Iter->CurrentState;
			TmpGoalState = Iter->GoalState;
		
			Iter = Iter->Parent;
		}
	}
	
	CreatePlanFromNodes(Agent, FinalNode, Plan);

	return Plan.Num() > 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
void FPlanner::CreatePlanFromNodes(IAgent& Agent, const FNode* Node, TArray<FPlanStep>& Plan)
{
	const FNode* Iter = Node;

	Plan.Empty();

	while (Iter->Action != nullptr)
	{
		const FNode* Parent = Iter->Parent;

		FPlanStep& Step = Plan.AddDefaulted_GetRef();
		Step.Action = Iter->Action;
		Step.WorldState = Parent->GoalState;

		//AI_LOG(TEXT(" %s"), *Step.Action->GetTypeName());

		Iter = Parent;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
FPlanner::FNode* FPlanner::Allocate()
{
	FNode* Node;
	if (NodePool.Num() == 0)
	{
		Node = new FNode();
	}
	else
	{
		const int32 LastIndex = NodePool.Num() - 1;
		Node = NodePool[LastIndex];
		NodePool.RemoveAt(LastIndex);
	}

	Node->Action = nullptr;
	Node->Parent = nullptr;

	Node->CurrentState.Clear();
	Node->GoalState.Clear();

	Node->CostFromStart = 0;
	Node->EstimatedCostToGoal = 0;
	Node->TotalCost = 0;

	return Node;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
void FPlanner::Free(FNode* Node)
{
	Node->Parent = nullptr;
	NodePool.Add(Node);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
void FPlanner::Merge(const FWorldState& AgentState, const FWorldState& GoalState, FWorldState& CurrentState)
{
	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);

		if (CurrentState.IsValid(Key))
			continue;

		const FWorldProperty* WorldPropertyGoal = GoalState.Get(Key);
		if (WorldPropertyGoal == nullptr)
			continue;

		const FWorldProperty* WorldPropertyAgent = AgentState.Get(Key);
		if (WorldPropertyAgent == nullptr)
			continue;

		CurrentState.Set(Key, WorldPropertyAgent);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
void FPlanner::AddToOpenList(FNode* Node)
{
	OpenList.Push(Node);
	OpenList.Sort();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
FPlanner::FNode* FPlanner::PopFromOpenList()
{
	if (OpenList.Num() == 0)
		return nullptr;

	const int32 LastIndex = OpenList.Num() - 1;
	FNode* Node = OpenList[LastIndex];
	OpenList.RemoveAt(LastIndex);

	return Node;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
bool FPlanner::IsPlanValid(IAgent* Agent, const FNode* Node)
{
	if (Node == nullptr || Node->Action == nullptr)
		return false;

	FWorldState WorldState;
	const FNode* Iter = Node;
	const FNode* Parent = nullptr;
	Merge(Agent->GetWorldState(), Iter->CurrentState, WorldState);

	EWorldPropertyKey FailedKey;
	while (Iter->Action != nullptr)
	{
		Parent = Iter->Parent;
		const UAbstractAction* Action = Iter->Action;

		if (!Action->CanSolveWorldState(WorldState, Parent->GoalState))
			return false;

		if (!Action->ArePreconditionsSatisfied(WorldState, Parent->GoalState, FailedKey))
			return false;

		if (!Action->AreContextPreconditionsSatisfied(*Agent, Parent->GoalState, true))
			return false;

		Action->ApplyResults(*Agent, WorldState, Parent->GoalState);

		Iter = Parent;
	}

	const int32 NumUnsatisfied = Parent->GoalState.CountUnsatisfied(WorldState);
	if (NumUnsatisfied > 0)
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
void FPlanner::UpdateNeighborActions(IAgent* Agent, const FNode* Node)
{
	NeighborActions.Empty();

	const FWorldState& WorldStateCurrent = Node->CurrentState;
	const FWorldState& WorldStateGoal = Node->GoalState;

	for (int32 KeyIndex = 0; KeyIndex < WorldPropertyKeyCount; ++KeyIndex)
	{
		const EWorldPropertyKey Key = static_cast<EWorldPropertyKey>(KeyIndex);

		const FWorldProperty* WorldPropertyCurrent = WorldStateCurrent.Get(Key);
		const FWorldProperty* WorldPropertyGoal = WorldStateGoal.Get(Key);

		if (WorldPropertyCurrent && WorldPropertyGoal)
		{
			if (WorldPropertyCurrent->Value != WorldPropertyGoal->Value ||
				WorldPropertyCurrent->Type != WorldPropertyGoal->Type)
			{
				const TArray<const UAbstractAction*>& Actions = Agent->GetActions(Key);
				for (int32 ActionIndex = 0; ActionIndex < Actions.Num(); ++ActionIndex)
				{
					const UAbstractAction* Action = Actions[ActionIndex];
					if (Action != Node->Action)
					{
						if (!NeighborActions.Contains(Action))
						{
							if (Action->AreContextPreconditionsSatisfied(*Agent, WorldStateGoal, true))
							{
								NeighborActions.Add(Action);
							}
						}
					}
				}
			}
		}
	}

	if (NeighborActions.Num() > 1)
	{
		NeighborActions.Sort([](const UAbstractAction& a, const UAbstractAction& b)
		{
			return a.GetPrecedence() > b.GetPrecedence();
		});
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
int32 FPlanner::CalculateCostFromStart(IAgent* Agent, const FNode* Node, FNode* NextNode)
{
	NextNode->CurrentState = Node->CurrentState;
	NextNode->GoalState = Node->GoalState;
	return Node->CostFromStart + NextNode->Action->GetCost(*Agent, NextNode->GoalState);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
int32 FPlanner::EstimateCostToGoal(IAgent* Agent, FNode* Node)
{
	Node->Action->SolveWorldState(Node->CurrentState, Node->GoalState);
	Node->Action->ApplyPreconditions(*Agent, Node->GoalState);

	Merge(Agent->GetWorldState(), Node->GoalState, Node->CurrentState);

	return Node->CurrentState.CountDifferences(Node->GoalState);
}
