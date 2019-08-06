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

#ifndef OPTFRAME_MOVE_COST_HPP_
#define OPTFRAME_MOVE_COST_HPP_

#include <cstdlib>
#include <iostream>
#include <cmath>

#include "Component.hpp"

using namespace std;

namespace optframe
{

class MoveCost: public Component
{
protected:
	// objective function value (cost difference)
	evtype objFunction;
	// infeasibility measure value (cost difference)
	evtype infMeasure;
	// constant to mutiply infeasibility weight
	evtype weight;
	// flag indicating if Evaluation was outdated
	bool outdated;
	// flag indicating if cost is estimated
	bool estimated;
	// storing costs for lexicographic approaches
	vector<pair<evtype, evtype> > alternatives;


public:
	explicit MoveCost(evtype obj, evtype inf = 0, evtype w = 1, bool _outdated = true, bool _estimated = false) :
			objFunction(obj), infMeasure(inf), weight(w), outdated(_outdated), estimated(_estimated)
	{
	}

	MoveCost(const MoveCost& mc) :
			objFunction(mc.objFunction), infMeasure(mc.infMeasure),
			weight(mc.weight), outdated(mc.outdated), estimated(mc.estimated),
			alternatives(mc.alternatives)
	{
	}

	virtual ~MoveCost()
	{
	}

	bool isEstimated() const
	{
		return estimated;
	}

	const vector<pair<evtype, evtype> >& getAlternativeCosts() const
	{
		return alternatives;
	}

	evtype getObjFunctionCost() const
	{
		return objFunction;
	}

	evtype getInfMeasureCost() const
	{
		return infMeasure;
	}

	void addAlternativeCost(const pair<evtype, evtype>& alternativeCost)
	{
		alternatives.push_back(alternativeCost);
	}

	void setAlternativeCosts(const vector<pair<evtype, evtype> >& alternativeCosts)
	{
		alternatives = alternativeCosts;
	}

	void setObjFunctionCost(evtype obj)
	{
		objFunction = obj;
	}

	void setInfMeasureCost(evtype inf)
	{
		infMeasure = inf;
	}

	evtype cost() const
	{
		return objFunction + weight * infMeasure;
	}

	// update Evaluation with costs
	virtual void update(Evaluation& e)
	{
		// update objective function value
		e.setObjFunction(e.getObjFunction()+objFunction);
		// update infeasibility measure value
		e.setInfMeasure(e.getInfMeasure()+infMeasure);
		// restore previous 'outdated' status, if Evaluation wasn't outdated before
		if(!outdated)
			e.outdated = outdated;

		// may also update lexicographic costs...
	}

	static string idComponent()
	{
		return "OptFrame:MoveCost";
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << fixed; // disable scientific notation
		cout << "Move cost = " << cost() << endl;
		if (alternatives.size() > 0)
		{
			cout << "Alternative Costs: ";
			for (unsigned i = 0; i < alternatives.size(); i++)
				cout << "(" << alternatives[i].first << ";" << alternatives[i].second << ") ";
			cout << endl;
		}
	}

	virtual MoveCost& operator=(const MoveCost& mc)
	{
		if (&mc == this) // auto ref check
			return *this;

		objFunction  = mc.objFunction;
		infMeasure   = mc.infMeasure;
		outdated     = mc.outdated;
		estimated    = mc.estimated;
		alternatives = mc.alternatives;
		weight       = mc.weight;

		return *this;
	}

	virtual MoveCost& clone() const
	{
		return *new MoveCost(*this);
	}

	friend class MultiMoveCost; // TODO: remove! experiment for MO problems
};

}

#endif /*OPTFRAME_MOVE_COST_HPP_*/

