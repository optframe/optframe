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

#ifndef OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_
#define OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_

#include "../../Solution.hpp"
#include "../../Constructive.hpp"

#include "GRASPFamily.h"

namespace optframe
{

// Greedy Randomized Constructive
template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class GRConstructive: public Constructive<R,ADS>, public GRASP
{
public:
	virtual ~GRConstructive()
	{
	}

	virtual Solution<R, ADS>* generateGRSolution(float alpha, double timelimit) = 0;

	virtual Solution<R, ADS>* generateSolution(double timelimit)
	{
		return generateGRSolution(1.0, timelimit);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << GRASP::family() << ":GRConstructive";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

// BasicGRConstructive can envelop a Constructive
template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicGRConstructive: public GRConstructive<R, ADS>
{
public:
	Constructive<R,ADS>& c;

	BasicGRConstructive(Constructive<R,ADS>& _c) : c(_c)
	{
	}

	virtual ~BasicGRConstructive()
	{
	}

	virtual Solution<R, ADS>* generateGRSolution(float alpha, double timelimit)
	{
		// ignoring alpha
		return c.generateSolution(timelimit);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << GRASP::family() << ":GRConstructive";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};



}

#endif /*OPTFRAME_GREEDY_RANDOMIZED_CONSTRUCTIVE_H_*/
