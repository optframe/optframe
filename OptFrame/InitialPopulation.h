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

#ifndef OPTFRAME_INITIALPOPULATION_H_
#define OPTFRAME_INITIALPOPULATION_H_

#include "Population.hpp"

#include "Component.hpp"
#include "Constructive.hpp"
#include "Heuristics/GRASP/GRConstructive.hpp"
#include "RandGen.hpp"

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class InitialPopulation: public Component
{
public:

	virtual ~InitialPopulation()
	{
	}

	virtual Population<R, ADS> generatePopulation(unsigned populationSize, double timelimit) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":InitialPopulation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicInitialPopulation: public InitialPopulation<R, ADS>
{
public:

	Constructive<R, ADS>& constructive;

	BasicInitialPopulation(Constructive<R, ADS>& _constructive) :
			constructive(_constructive)
	{
	}

	virtual ~BasicInitialPopulation()
	{
	}

	virtual Population<R, ADS> generatePopulation(unsigned populationSize, double timelimit)
	{
		Population<R, ADS>* p = new Population<R, ADS>;
		for (unsigned i = 0; i < populationSize; i++)
			p->push_back(constructive.generateSolution(timelimit));
		return *p;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Population<R, ADS>::idComponent() << ":BasicInitialPopulation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class GRInitialPopulation: public InitialPopulation<R, ADS>
{
public:
	GRConstructive<R, ADS>& constructive;
	RandGen& rg;
	double maxAlpha; // limit the solution to be not so random

	GRInitialPopulation(GRConstructive<R, ADS>& _constructive, RandGen& _rg, double _maxAlpha) :
			constructive(_constructive), rg(_rg), maxAlpha(_maxAlpha)
	{
	}

	virtual ~GRInitialPopulation()
	{
	}

	virtual Population<R, ADS> generatePopulation(unsigned populationSize, double timelimit)
	{
		Population<R, ADS>* p = new Population<R, ADS>;
		for (unsigned i = 0; i < populationSize; i++)
		{
			float alpha = rg.rand01();
			while (alpha > maxAlpha)
			{
				alpha = rg.rand01();
			}

			if (alpha == 0)
				alpha = 0.00001;
			p->push_back(constructive.generateGRSolution(alpha, timelimit));
		}
		return *p;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Population<R, ADS>::idComponent() << ":GRInitialPopulation";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_INITIALPOPULATION_H_*/
