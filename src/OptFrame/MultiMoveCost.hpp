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

#ifndef OPTFRAME_MULTI_MOVE_COST_HPP_
#define OPTFRAME_MULTI_MOVE_COST_HPP_

#include <cstdlib>
#include <iostream>
#include <cmath>

#include "Component.hpp"
#include "BaseSolution.h" // Base concepts
#include "Evaluation.hpp"
#include "MoveCost.hpp"

using namespace std;

namespace optframe
{

//// more than 'totally_ordered' we need to ensure arithmetics here... TODO: see that (same as Evaluation)
template<optframe::totally_ordered ObjType = evtype, XEvaluation XEv = Evaluation<ObjType>>
//template<class ObjType = evtype, XEvaluation XEv = Evaluation<ObjType>>
class MultiMoveCost: public Component
{
protected:
	vector<MoveCost<ObjType, XEv>*> vmc;

public:
	explicit MultiMoveCost(vector<MoveCost<>*> _vmc) :
			vmc(_vmc)
	{
	}

	MultiMoveCost(const MultiMoveCost<>& mc) :
			vmc(mc.vmc)
	{
	}

	virtual ~MultiMoveCost()
	{
	}

	int size() const
	{
		return vmc.size();
	}

	bool hasCost(int k) const
	{
		return vmc[k];
	}

	bool isEstimated(int k) const
	{
		return vmc[k]->estimated;
	}

	const vector<pair<ObjType, ObjType> >& getAlternativeCosts(int k) const
	{
		return vmc[k]->alternatives;
	}

	ObjType getObjFunctionCost(int k) const
	{
		return vmc[k]->objFunction;
	}

	ObjType getInfMeasureCost(int k) const
	{
		return vmc[k]->infMeasure;
	}

	void addAlternativeCost(const pair<ObjType, ObjType>& alternativeCost, int k)
	{
		vmc[k]->alternatives.push_back(alternativeCost);
	}

	void setAlternativeCosts(const vector<pair<ObjType, ObjType> >& alternativeCosts, int k)
	{
		vmc[k]->alternatives = alternativeCosts;
	}

	void setObjFunctionCost(ObjType obj, int k)
	{
		vmc[k]->objFunction = obj;
	}

	void setInfMeasureCost(ObjType inf, int k)
	{
		vmc[k]->infMeasure = inf;
	}

	ObjType cost(int k) const
	{
		return vmc[k]->cost();
	}

	static string idComponent()
	{
		return "OptFrame:MultiMoveCost";
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << fixed; // disable scientific notation
		cout << "MultiMoveCost for " << size() << " objectives:" << endl;
		for(unsigned i=0; i<vmc.size(); i++)
			if(vmc[i])
				vmc[i]->print();
			else
				cout << "NO COST" << endl;
	}

	virtual MultiMoveCost<>& operator=(const MultiMoveCost<>& mmc)
	{
		if (&mmc == this) // auto ref check
			return *this;

		vmc = mmc.vmc; // TODO fix: this should handle some local instances, for the future...

		return *this;
	}

	virtual MultiMoveCost<>& clone() const
	{
		return *new MultiMoveCost<>(*this);
	}
};

#ifndef NDEBUG
struct optframe_debug_test_multimove_cost
{
   MultiMoveCost<> testMoveCost;
};
#endif

} // namespace optframe

#endif /*OPTFRAME_MULTI_MOVE_COST_HPP_*/
