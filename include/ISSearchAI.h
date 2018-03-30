#ifndef __ISSEARCHAI_H
#define __ISSEARCHAI_H

#include <ISTypes.h>


//-----------------------------------------------------------------------------
// TYPES
//-----------------------------------------------------------------------------

namespace AI
{
	typedef UINT32 Action;
	typedef std::vector<Action>* Path;

	struct State {};
	typedef std::shared_ptr<State> StatePtr;
	struct SearchProblem
	{
		virtual StatePtr getStartState() = 0;
		virtual bool isGoalState(StatePtr state) const = 0;
		virtual bool isActionLegal(StatePtr state, Action action) const = 0;
		virtual StatePtr generateSuccessor(StatePtr state, Action action, double* cost = NULL) const = 0;
		virtual StatePtr nextSuccessor(StatePtr state, Action& lastaction, double* cost = NULL) const = 0;
	};

	typedef double (CALLBACK* Heuristic)(StatePtr state, SearchProblem& problem);
	typedef bool (*StatePtrComparator)(StatePtr ptr0, StatePtr ptr1);
}

//-----------------------------------------------------------------------------
// PREDEFINED HEURISTICS
//-----------------------------------------------------------------------------

double __stdcall NullHeuristic(std::shared_ptr<AI::State> state, AI::SearchProblem& problem);

//-----------------------------------------------------------------------------
// SEARCH FUNCTIONS
//-----------------------------------------------------------------------------

AI::Path DepthFirstSearch(AI::SearchProblem& problem, AI::StatePtrComparator comp);
AI::Path BreadthFirstSearch(AI::SearchProblem& problem, AI::StatePtrComparator comp);
AI::Path UniformCostSearch(AI::SearchProblem& problem, AI::StatePtrComparator comp);
AI::Path AStarSearch(AI::SearchProblem& problem, AI::StatePtrComparator comp, AI::Heuristic heuristic = NullHeuristic);

#endif