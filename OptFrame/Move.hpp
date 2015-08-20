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

//#include "Action.hpp"

using namespace std;

typedef void OPTFRAME_DEFAULT_PROBLEM;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Move : public Component
{
public:

	virtual ~Move()
	{
	}

	bool canBeApplied(const Solution<R, ADS>& s)
	{
		return canBeApplied(s.getR(), s.getADS());
	}

//protected: // TODO: protect...
	virtual bool canBeApplied(const R&, const ADS&) = 0;

	// returns true if the apply returns a non-null pointer
	virtual bool hasReverse()
	{
		return true; // TODO: make it pure virtual "= 0"
	}

public:
	Move<R, ADS>* apply(Solution<R, ADS>& s)
	{
		return apply(s.getR(), s.getADS());
	}

	Move<R, ADS>* apply(Evaluation& e, Solution<R, ADS>& s)
	{
		return apply(e, s.getR(), s.getADS());
	}

////protected:
	virtual Move<R, ADS>* apply(R& r, ADS& ads) = 0;

	virtual Move<R, ADS>* apply(Evaluation& e, R& r, ADS& ads)
	{
		Move<R, ADS>* rev = apply(r, ads);
		updateNeighStatus(ads);
		return rev;
	}

	// TODO: coming in one of the next versions..
	//virtual pair<Move<R, ADS>&, MoveCost*> apply(const Evaluation& e, R& r, ADS& ads) = 0;

public:

	// TODO: deprecated
	virtual void updateNeighStatus(ADS& ads)
	{
	}

	// ================== move independence and local search marking

	virtual bool isIndependent(const Move<R, ADS>& m)
	{
	    // example: in VRP, move1 changes one route and move2 changes another... they are independent.
	    // move1.isIndependent(move2) should return true.
	    // by default, it is false (no move is independent)
	    return false;
	}


	virtual bool isPartialLocalOptimum(const Solution<R, ADS>& s)
	{
	    // the idea is to use this flag to ignore moves that are useless,
	    // given that the solution is already in a (full) local optimum (or partial).

	    return false;
	}

	// ================== cost calculation

	virtual MoveCost* cost(const Evaluation& e, const R& r, const ADS& ads)
	{
		return NULL;
	}

	virtual bool operator==(const Move<R, ADS>& m) const = 0;

	bool operator!=(const Move<R, ADS>& m) const
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
