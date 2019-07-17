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

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class Move : public Component
{
public:

	virtual ~Move()
	{
	}

	bool canBeAppliedToSolution(const S& s)
	{
		return canBeApplied(s.getR(), s.getADSptr());
	}

	virtual bool canBeApplied(const R&, const ADS*) = 0;

	// returns true if the apply returns a non-null pointer
	virtual bool hasReverse()
	{
		return true; // TODO: make it pure virtual "= 0"
	}

	Move<R, ADS, S>* applySolution(S& s)
	{
		return apply(s.getR(), s.getADSptr());
	}

	Move<R, ADS, S>* applyUpdateSolution(Evaluation& e, S& s)
	{
		return applyUpdate(e, s.getR(), s.getADSptr());
	}

	Move<R, ADS, S>* applyMEVUpdateSolution(MultiEvaluation& mev, S& s)
	{
		return applyMEV(mev, s.getR(), s.getADSptr());
	}

	virtual Move<R, ADS, S>* apply(R& r, ADS* ads) = 0;

	virtual Move<R, ADS, S>* applyUpdate(Evaluation& e, R& r, ADS* ads)
	{
		// boolean 'outdated' indicates that Evaluation needs update (after Solution change)
		// note that even if the reverse move is applied, the Evaluation will continue with
		// the outdated status set to true. So more efficient approaches may rewrite this
		// method, or implement efficient re-evaluation by means of the 'cost' method.
		e.outdated = true;
		// apply the move to R and ADS, saving the reverse (or undo) move
		Move<R, ADS, S>* rev = apply(r, ads);
		// update neighborhood local optimum status TODO:deprecated
		updateNeighStatus(ads);
		// return reverse move (or null)
		return rev;
	}

	virtual Move<R, ADS, S>* applyMEV(MultiEvaluation& mev, R& r, ADS* ads)
	{
		// boolean 'outdated' indicates that Evaluation needs update (after Solution change)
		// note that even if the reverse move is applied, the Evaluation will continue with
		// the outdated status set to true. So more efficient approaches may rewrite this
		// method, or implement efficient re-evaluation by means of the 'cost' method.
		for (unsigned nE = 0; nE < mev.size(); nE++)
			mev.setOutdated(nE,true);
		// apply the move to R and ADS, saving the reverse (or undo) move
		Move<R, ADS, S>* rev = apply(r, ads);
		// update neighborhood local optimum status TODO:deprecated
		updateNeighStatus(ads);
		// return reverse move (or null)
		return rev;
	}

	// TODO: coming in one of the next versions..
	//virtual pair<Move<R, ADS, S>&, MoveCost*> apply(const Evaluation& e, R& r, ADS& ads) = 0;

	// ================== cost calculation

	virtual MoveCost* cost(const Evaluation& e, const R& r, const ADS* ads, bool allowEstimated)
	{
		return nullptr;
	}

	// experiment for multi objective problems
	virtual MultiMoveCost* costMEV(const MultiEvaluation& mev, const R& r, const ADS* ads, bool allowEstimated)
	{
		return nullptr;
	}

	// ================== move independence and local search marking

	virtual bool independentOf(const Move<R, ADS, S>& m)
	{
	    // example: in VRP, move1 changes one route and move2 changes another... they are independent.
	    // move1.isIndependent(move2) should return true.
	    // by default, it is false (no move is independent)
	    return false;
	}

	// ================== local search marking

	// TODO: deprecated. replaced by updateLOS?
	virtual void updateNeighStatus(ADS* ads)
	{
	}

	// TODO: force before apply(R,ADS) and after apply(S)?
	virtual void updateLOS(R& r, ADS& ads, Evaluation& e)
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

	virtual bool operator==(const Move<R, ADS, S>& m) const = 0;

	bool operator!=(const Move<R, ADS, S>& m) const
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
