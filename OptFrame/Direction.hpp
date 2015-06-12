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

#ifndef OPTFRAME_OPTIMIZATION_DIRECTION_HPP_
#define OPTFRAME_OPTIMIZATION_DIRECTION_HPP_

#include <float.h>
#include <limits>

#include "Solution.hpp"
#include "ADSManager.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"
#include "MoveCost.hpp"

#include <iostream>

#include "Component.hpp"
#include "Action.hpp"

using namespace std;
using namespace scannerpp;

namespace optframe
{

class Direction: public Component
{
public:

	virtual ~Direction()
	{
	}

	// ============ betterThan ===========

	//! abstract method betterThan: true when a < b for minimization problems; and true when a > b for maximization problems.
	/*!
	 betterThan is the method in OptFrame used to guide the search methods into the solution space.
	 with betterThan the search methods are able to compare good and poor solutions, in one of the two directions: minimization and maximization.
	 It must be implemented by the final user in one of these ways:
	 - for minimization problems, returns a < b;
	 - for maximization problems, returns a > b.
	 */
	virtual bool betterThan(double a, double b) = 0;

	// true if 'mc1' is better than 'mc2'
	virtual inline bool betterThan(const MoveCost& mc1, const MoveCost& mc2)
	{
		return betterThan(mc1.cost(), mc2.cost());
	}

	// true if 'e1' is better than 'e2'
	virtual inline bool betterThan(const Evaluation& e1, const Evaluation& e2)
	{
		return betterThan(e1.evaluation(), e2.evaluation());
	}

	virtual inline bool betterThan(const vector<pair<double, double> >& altCosts1, const vector<pair<double, double> >& altCosts2)
	{
		if(altCosts1.size() != altCosts2.size())
			return false;
		for(unsigned i = 0; i < altCosts1.size(); i++)
			if(!betterThan(altCosts1[i].first + altCosts1[i].second, altCosts2[i].first + altCosts2[i].second))
				return false;
		return true;
	}

	// ============ betterOrEquals ===========

	inline bool betterOrEquals(const vector<pair<double, double> >& altCosts1, const vector<pair<double, double> >& altCosts2)
	{
		return betterThan(altCosts1, altCosts2) || equals(altCosts1, altCosts2);
	}

	inline bool betterOrEquals(const MoveCost& mc1, const MoveCost& mc2)
	{
		return betterThan(mc1, mc2) || equals(mc1, mc2);
	}

	inline bool betterOrEquals(const Evaluation& e1, const Evaluation& e2)
	{
		return betterThan(e1, e2) || equals(e1, e2);
	}

	inline bool betterOrEquals(double a, double b)
	{
		return betterThan(a, b) || equals(a, b);
	}

	// ============ equals ============

	virtual inline bool equals(const vector<pair<double, double> >& altCosts1, const vector<pair<double, double> >& altCosts2)
	{
		if(altCosts1.size() != altCosts2.size())
			return false;
		for(unsigned i = 0; i < altCosts1.size(); i++)
			if(!equals(altCosts1[i].first + altCosts1[i].second, altCosts2[i].first + altCosts2[i].second))
				return false;
		return true;
	}

	virtual inline bool equals(const MoveCost& mc1, const MoveCost& mc2)
	{
		return equals(mc1.cost(), mc2.cost());
	}

	virtual inline bool equals(const Evaluation& e1, const Evaluation& e2)
	{
		return equals(e1.evaluation(), e2.evaluation());
	}

	virtual inline bool equals(double a, double b)
	{
		return (abs(a - b) < OPTFRAME_EPSILON);
	}

	// ============= improvement =============

	virtual bool isImprovement(const MoveCost& mc, const Evaluation& e1, const Evaluation& e2)
	{
		double ec1 = mc.cost() + e1.evaluation();
		if(betterThan(ec1, e2.evaluation()))
			return true;
		else if(equals(ec1, e2.evaluation()))
		{
			if(e1.getAlternativeCosts().size() != e2.getAlternativeCosts().size())
			{
				cout << "Evaluator Error: |e1.alternatives|=" << e1.getAlternativeCosts().size() << " |e2.alternatives|=" << e2.getAlternativeCosts().size();
				cout << endl;
				exit(1);
				return false;
			}

			if(mc.getAlternativeCosts().size() != e1.getAlternativeCosts().size())
			{
				cout << "Evaluator Error: |mc.alternatives|=" << mc.getAlternativeCosts().size() << " |e1.alternatives|=" << e1.getAlternativeCosts().size();
				cout << endl;
				exit(1);
				return false;
			}

			vector<pair<double, double> > altCosts1(e1.getAlternativeCosts());
			for(unsigned i = 0; i < altCosts1.size(); i++)
			{
				altCosts1[i].first += mc.getAlternativeCosts()[i].first;
				altCosts1[i].second += mc.getAlternativeCosts()[i].second;
			}
			return betterThan(altCosts1, e2.getAlternativeCosts());
		}
		else
			return false;
	}

	virtual inline bool isImprovement(const MoveCost& mc)
	{
		return betterThan(mc.cost(), 0);
	}

	// ============= direction ==============

	virtual bool isMinimization() const = 0;

	inline bool isMaximization() const
	{
		return !isMinimization();
	}

	// ============ estimation =============

	inline double worst()
	{
		if(isMinimization())
			return max();
		else
			return min();
	}

	// bad approximation!
	virtual inline double min()
	{
		////return -DBL_MAX;

		if(numeric_limits<double>::has_infinity)
			return -numeric_limits<double>::infinity();
		else
			return -numeric_limits<double>::max();
	}

	// bad approximation!
	virtual inline double max()
	{
		////return DBL_MAX;

		if(numeric_limits<double>::has_infinity)
			return numeric_limits<double>::infinity();
		else
			return numeric_limits<double>::max();
	}

	// ============= Component ===============

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << "Direction";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		if(isMinimization())
			return "Direction:MIN";
		else
			return "Direction:MAX";
	}

	virtual void print() const
	{
		cout << toString() << endl;
	}

};

class Minimization: public Direction
{
public:

	virtual ~Minimization()
	{
	}

	inline bool betterThan(double f1, double f2)
	{
		// MINIMIZATION
		return (f1 < (f2 - OPTFRAME_EPSILON));
	}

	inline bool isMinimization() const
	{
		return true;
	}
};

class Maximization: public Direction
{
public:

	virtual ~Maximization()
	{
	}

	inline bool betterThan(double f1, double f2)
	{
		// MAXIMIZATION
		return (f1 > (f2 + OPTFRAME_EPSILON));
	}

	inline bool isMinimization() const
	{
		return false;
	}
};

}

#endif /*OPTFRAME_OPTIMIZATION_DIRECTION_HPP_*/
