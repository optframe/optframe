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
	double objFunction;
	double infMeasure;

	bool estimated;

	vector<pair<double, double> > alternatives; // for lexicographic approaches

public:
	explicit MoveCost(double obj, double inf = 0.0, bool est = false) :
			objFunction(obj), infMeasure(inf), estimated(est)
	{
	}

	MoveCost(const MoveCost& mc) :
			objFunction(mc.objFunction), infMeasure(mc.infMeasure), estimated(mc.estimated), alternatives(mc.alternatives)
	{
	}

	virtual ~MoveCost()
	{
	}

	bool isEstimated() const
	{
		return estimated;
	}

	const vector<pair<double, double> >& getAlternativeCosts() const
	{
		return alternatives;
	}

	double getObjFunctionCost() const
	{
		return objFunction;
	}

	double getInfMeasureCost() const
	{
		return infMeasure;
	}

	void addAlternativeCost(const pair<double, double>& alternativeCost)
	{
		alternatives.push_back(alternativeCost);
	}

	void setAlternativeCosts(const vector<pair<double, double> >& alternativeCosts)
	{
		alternatives = alternativeCosts;
	}

	void setObjFunctionCost(double obj)
	{
		objFunction = obj;
	}

	void setInfMeasureCost(double inf)
	{
		infMeasure = inf;
	}

	double cost() const
	{
		return objFunction + infMeasure;
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

		objFunction = mc.objFunction;
		infMeasure = mc.infMeasure;
		estimated = mc.estimated;
		alternatives = mc.alternatives;

		return *this;
	}

	virtual MoveCost& clone() const
	{
		return *new MoveCost(*this);
	}
};

}

#endif /*OPTFRAME_MOVE_COST_HPP_*/
