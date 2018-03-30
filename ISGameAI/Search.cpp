#include "ISSearchAI.h"
#include <set>

using namespace AI;

Path DepthFirstSearch(SearchProblem& problem, StatePtrComparator comp)
{
	std::vector<Action>* actions = new std::vector<Action>();

	struct StateAndCostAndDir
	{
		StatePtr state;
		int cost;
		Action action;
		StateAndCostAndDir(StatePtr state, int cost, Action action) : state(state), cost(cost), action(action) {}
	};
	std::stack<StateAndCostAndDir> fringe;
	std::set<StatePtr, bool(*)(StatePtr,StatePtr)> closed(comp);

	// Create start node with empty path
	StateAndCostAndDir startnode(problem.getStartState(), -1, (Action)-1);
	StateAndCostAndDir& node = startnode;
	if(problem.isGoalState(node.state))
		return actions;
	closed.insert(node.state);

	while(1)
	{
		StatePtr successor;
		Action successor_action = (Action)-1;
		while((successor = problem.nextSuccessor(node.state, successor_action)) != NULL)
		{
			// Test for goal state (before-push)
			if(problem.isGoalState(successor))
			{
				actions->push_back(successor_action);
				return actions; // Goal found
			}

			// Avoid already pushed states (graph search)
			std::set<StatePtr, bool(*)(StatePtr,StatePtr)>::const_iterator iter = closed.find(successor);
			if(iter == closed.end())
			{
				fringe.push(StateAndCostAndDir(successor, node.cost + 1, successor_action));
				closed.insert(successor);
			}
		}

		// Test if maze has been completly traversed without finding any goal state
		if(fringe.empty())
			return actions; // Goal not found

		// Pop new node and update dirs to be the path of the freshly pop'ed node
		node = fringe.top();
		fringe.pop();
		if(node.cost)
			actions->assign(&actions->front(), &actions->front() + node.cost);
		else
			actions->clear();
		actions->push_back(node.action);
	}
}

Path BreadthFirstSearch(SearchProblem& problem, StatePtrComparator comp)
{
	struct StateAndPath
	{
		StatePtr state;
		std::vector<Action>* actions;
		StateAndPath(StatePtr state, std::vector<Action>* actions) : state(state), actions(actions) {}
	};
	std::queue<StateAndPath> fringe;
	std::set<StatePtr, bool(*)(StatePtr,StatePtr)> closed(comp);

	StateAndPath startnode(problem.getStartState(), new std::vector<Action>());
	StateAndPath& node = startnode;
	if(problem.isGoalState(node.state))
		return startnode.actions; // Start state is goal
	closed.insert(node.state);

	while(1)
	{
		StatePtr successor;
		Action successor_action = (Action)-1;
		while((successor = problem.nextSuccessor(node.state, successor_action)) != NULL)
		{
			std::set<StatePtr, bool(*)(StatePtr,StatePtr)>::const_iterator iter = closed.find(successor);
			if(iter == closed.end())
			{
				std::vector<Action>* newactions = node.actions->empty() ? new std::vector<Action>() : new std::vector<Action>(node.actions->begin(), node.actions->end());
				newactions->push_back(successor_action);
				fringe.push(StateAndPath(successor, newactions));
				closed.insert(successor);
			}
		}

		if(fringe.empty())
			return new std::vector<Action>(); // Goal not found
		delete node.actions;
		node = fringe.front();
		fringe.pop();
		if(problem.isGoalState(node.state))
		{
			while(fringe.size())
			{
				delete fringe.front().actions;
				fringe.pop();
			}
			return node.actions; // Goal found
		}
	}
}

Path UniformCostSearch(SearchProblem& problem, StatePtrComparator comp)
{
	struct StateAndPathAndCost
	{
		StatePtr state;
		std::vector<Action>* actions;
		double cost;
		StateAndPathAndCost(StatePtr state, std::vector<Action>* actions, double cost) : state(state), actions(actions), cost(cost) {}
		struct CompareFunctor
		{
			bool operator()(const StateAndPathAndCost& left, const StateAndPathAndCost& right) const
				{return left.cost < right.cost;}
		};
	};
	PriorityQueue<StateAndPathAndCost, StateAndPathAndCost::CompareFunctor> fringe;
	std::map<StatePtr, double, bool(*)(StatePtr,StatePtr)> closed(comp);

	StateAndPathAndCost startnode(problem.getStartState(), new std::vector<Action>(), 0.0);
	StateAndPathAndCost& node = startnode;
	if(problem.isGoalState(node.state))
		return startnode.actions; // Start state is goal
	closed[node.state] = node.cost;

	while(1)
	{
		StatePtr successor;
		Action successor_action = (Action)-1;
		double successor_cost;
		while((successor = problem.nextSuccessor(node.state, successor_action, &successor_cost)) != NULL)
		{
			std::map<StatePtr, double, bool(*)(StatePtr,StatePtr)>::const_iterator iter = closed.find(successor);
			if(iter == closed.end() || node.cost + successor_cost < iter->second)
			{
				std::vector<Action>* newactions = node.actions->empty() ? new std::vector<Action>() : new std::vector<Action>(node.actions->begin(), node.actions->end());
				newactions->push_back(successor_action);
				fringe.insert(StateAndPathAndCost(successor, newactions, node.cost + successor_cost));
				closed[successor] = node.cost + successor_cost;
			}
		}

		if(fringe.empty())
			return new std::vector<Action>(); // Goal not found
		delete node.actions;
		node = fringe.front();
		fringe.pop_front();
		if(problem.isGoalState(node.state))
		{
			while(fringe.size())
			{
				delete fringe.front().actions;
				fringe.pop_front();
			}
			return node.actions; // Goal found
		}
	}
}

double __stdcall NullHeuristic(StatePtr state, SearchProblem& problem)
{
	return 0.0;
}

Path AStarSearch(SearchProblem& problem, StatePtrComparator comp, Heuristic heuristic)
{
	struct StateAndPathAndCost
	{
		StatePtr state;
		std::vector<Action>* actions;
		double f, g;
		StateAndPathAndCost(StatePtr state, std::vector<Action>* actions, double f, double g) : state(state), actions(actions), f(f), g(g) {}
		struct CompareFunctor
		{
			bool operator()(const StateAndPathAndCost& left, const StateAndPathAndCost& right) const
				{return left.f < right.f;}
		};
	};
	PriorityQueue<StateAndPathAndCost, StateAndPathAndCost::CompareFunctor> fringe;
	std::map<StatePtr, double, bool(*)(StatePtr,StatePtr)> closed(comp);

	bool CHECK_CONSISTENCY = false;

	StateAndPathAndCost startnode(problem.getStartState(), new std::vector<Action>(), 0.0, 0.0);
	StateAndPathAndCost& node = startnode;
	if(problem.isGoalState(node.state))
		return startnode.actions; // Start state is goal
	closed[node.state] = node.f;

	while(1)
	{
		StatePtr successor;
		Action successor_action = (Action)-1;
		double successor_cost;
		while((successor = problem.nextSuccessor(node.state, successor_action, &successor_cost)) != NULL)
		{
			double h = heuristic(successor, problem);
			double new_g = node.g + successor_cost;
			if(CHECK_CONSISTENCY && new_g + h < node.f)
			{
				Result::PrintLogMessage("Inconsistency found in A*-search heuristic");
				while(fringe.size())
				{
					delete fringe.front().actions;
					fringe.pop_front();
				}
				return startnode.actions;
			}
			std::map<StatePtr, double, bool(*)(StatePtr,StatePtr)>::const_iterator iter = closed.find(successor);
			if(iter == closed.end() || new_g + h < iter->second)
			{
				std::vector<Action>* newactions = node.actions->empty() ? new std::vector<Action>() : new std::vector<Action>(node.actions->begin(), node.actions->end());
				newactions->push_back(successor_action);
				fringe.insert(StateAndPathAndCost(successor, newactions, new_g + h, new_g));
				closed[successor] = new_g + h;
			}
		}

		if(fringe.empty())
			return new std::vector<Action>(); // Goal not found
		delete node.actions;
		node = fringe.front();
		fringe.pop_front();
		if(problem.isGoalState(node.state))
		{
			while(fringe.size())
			{
				delete fringe.front().actions;
				fringe.pop_front();
			}
			return node.actions; // Goal found
		}
	}
}