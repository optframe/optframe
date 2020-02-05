// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_MOVE_HPP_
#define OPTFRAME_MOVE_HPP_

#include "Solution.hpp"
#include "Solutions/CopySolution.hpp"
#include "Evaluation.hpp"
#include "MoveCost.hpp"

#include "Component.hpp"

#include "MultiMoveCost.hpp"
#include "MultiEvaluation.hpp"

//#include "Action.hpp"

using namespace std;

typedef void OPTFRAME_DEFAULT_PROBLEM;

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>>
class Move : public Component
{
public:

	virtual ~Move()
	{
	}

	virtual bool canBeApplied(const S&) = 0;

	// returns true if the apply returns a non-null pointer
	virtual bool hasReverse()
	{
		return true; // TODO: make it pure virtual "= 0"
	}

	virtual Move<S, XEv>* apply(S& s) = 0;

	virtual Move<S, XEv>* applyUpdate(XEv& e, S& s)
	{
		// boolean 'outdated' indicates that Evaluation needs update (after Solution change)
		// note that even if the reverse move is applied, the Evaluation will continue with
		// the outdated status set to true. So more efficient approaches may rewrite this
		// method, or implement efficient re-evaluation by means of the 'cost' method.
		e.outdated = true;
		// apply the move to R and ADS, saving the reverse (or undo) move
		Move<S, XEv>* rev = apply(s);
		// update neighborhood local optimum status TODO:deprecated
		updateNeighStatus(s);

		// return reverse move (or null)
		return rev;
	}

	virtual Move<S, XEv>* applyMEV(MultiEvaluation<>& mev, S& s)
	{
		// boolean 'outdated' indicates that Evaluation needs update (after Solution change)
		// note that even if the reverse move is applied, the Evaluation will continue with
		// the outdated status set to true. So more efficient approaches may rewrite this
		// method, or implement efficient re-evaluation by means of the 'cost' method.
		for (unsigned nE = 0; nE < mev.size(); nE++)
			mev.setOutdated(nE,true);
		// apply the move to R and ADS, saving the reverse (or undo) move
		Move<S, XEv>* rev = apply(s);
		// update neighborhood local optimum status TODO:deprecated
		updateNeighStatus(s);
		// return reverse move (or null)
		return rev;
	}

	// TODO: coming in one of the next versions..
	//virtual pair<Move<S, XEv>&, MoveCost<>*> apply(const Evaluation<>& e, R& r, ADS& ads) = 0;

	// ================== cost calculation

	virtual MoveCost<>* cost(const Evaluation<>& e, const S& s, bool allowEstimated)
	{
		return nullptr;
	}

	// experiment for multi objective problems
	virtual MultiMoveCost<>* costMEV(const MultiEvaluation<>& mev, const S& s, bool allowEstimated)
	{
		return nullptr;
	}

	// ================== move independence and local search marking

	virtual bool independentOf(const Move<S, XEv>& m)
	{
	    // example: in VRP, move1 changes one route and move2 changes another... they are independent.
	    // move1.isIndependent(move2) should return true.
	    // by default, it is false (no move is independent)
	    return false;
	}

	// ================== local search marking

	// TODO: deprecated. replaced by updateLOS?
	//virtual void updateNeighStatus(ADS* ads)
	virtual void updateNeighStatus(S& s)
	{
	}

	// TODO: force before apply(R,ADS) and after apply(S)?
	virtual void updateLOS(S& s, XEv& e)
	{
	}

	// TODO: rethink!
	virtual bool isPartialLocalOptimum(const S& s)
	{
	    // the idea is to use this flag to ignore moves that are useless,
	    // given that the solution is already in a (full) local optimum (or partial).

	    return false;
	}

	// ================== basic comparison

	virtual bool operator==(const Move<S, XEv>& m) const = 0;

	bool operator!=(const Move<S, XEv>& m) const
	{
		return !(*this == m);
	}

   static string idComponent()
   {
		stringstream ss;
		ss << Component::idComponent() << ":Move";
		return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

	virtual void print() const = 0;
};

}

#endif /*OPTFRAME_MOVE_HPP_*/
