// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_MULTI_DIRECTION_HPP_
#define OPTFRAME_MULTI_DIRECTION_HPP_

#include <float.h>
#include <limits>

#include "Solution.hpp"
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

template<class DS = OPTFRAME_DEFAULT_DS>
class MultiDirection: public Component
{
protected:
	vector<Direction<DS>*> vDir;

public:
	unsigned nObjectives;

	MultiDirection(vector<Direction<DS>*>& _vDir)
	{
		for(unsigned i = 0; i < _vDir.size(); i++)
			if(_vDir[i])
				vDir.push_back(_vDir[i]);
		nObjectives = vDir.size();
	}

	MultiDirection(MultiDirection<DS>& mDir) :
			vDir(mDir.vDir), nObjectives(mDir.nObjectives)
	{
	}

	MultiDirection()
	{
		nObjectives = 0;
	}

	virtual ~MultiDirection()
	{
	}

	virtual MultiDirection<DS>& addObjective(Direction<DS>* ds)
	{
		if(ds)
			vDir.push_back(ds);
		nObjectives = vDir.size();

		return *this;
	}

	vector<Direction<DS>*>& getDirections()
	{
		return vDir;
	}

	// ============ betterThan ===========

	template<class T>
	inline bool betterThan(unsigned obj, const T& a, const T& b)
	{
		return vDir[obj]->betterThan(a, b);
	}

	// ============ betterOrEquals ===========

	template<class T>
	inline bool betterOrEquals(unsigned obj, const T& a, const T& b)
	{
		return vDir[obj]->betterOrEquals(obj, a, b);
	}

	// ============ equals ============

	template<class T>
	inline bool equals(unsigned obj, const T& a, const T& b)
	{
		return vDir[obj]->equals(a, b);
	}

	// ============= improvement =============

	inline bool isImprovement(unsigned obj, const MoveCost& mc, const Evaluation<DS>& e1, const Evaluation<DS>& e2)
	{
		return vDir[obj]->isImprovement(obj, mc, e1, e2);
	}

	inline bool isImprovement(unsigned obj, const MoveCost& mc)
	{
		return vDir[obj]->isImprovement(obj, mc);
	}

	// ============= direction ==============

	inline bool isMinimization(unsigned obj)
	{
		return vDir[obj]->isMinimization();
	}

	inline bool isMaximization(unsigned obj)
	{
		return vDir[obj]->isMaximization();
	}

	// ============ estimation =============

	inline double worst(unsigned obj)
	{
		return vDir[obj]->worst();
	}

	inline double min(unsigned obj)
	{
		return vDir[obj]->min();
	}

	inline double max(unsigned obj)
	{
		return vDir[obj]->max();
	}

	// ============= Component ===============

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":MultiDirection";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}

#endif /*OPTFRAME_MULTI_DIRECTION_HPP_*/
