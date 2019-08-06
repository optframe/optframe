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

#ifndef OPTFRAME_INITIALPARETO_HPP_
#define OPTFRAME_INITIALPARETO_HPP_

#include "Component.hpp"
#include "Constructive.hpp"
#include "Heuristics/GRASP/GRConstructive.hpp"
#include "RandGen.hpp"
#include "Pareto.hpp"
#include "MultiEvaluator.hpp"
//#include "MultiEvaluation.hpp"
//#include "Timer.hpp"

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class InitialPareto: public Component
{
public:

	virtual ~InitialPareto()
	{
	}

	virtual Pareto<R, ADS> generatePareto(unsigned populationSize, double timeLimit = 10000000) = 0;


	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":InitialPareto";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicInitialPareto: public InitialPareto<R, ADS>
{
public:

	Constructive<R, ADS>& constructive;
	paretoManager<R, ADS> pMan;

	BasicInitialPareto(Constructive<R, ADS>& _constructive, MultiEvaluator<R, ADS>& _mev) :
			constructive(_constructive), pMan(paretoManager<R, ADS>(_mev))
	{
	}

	virtual ~BasicInitialPareto()
	{
	}

	virtual Pareto<R, ADS> generatePareto(unsigned populationSize, double timelimit = 100000000)
	{
		Pareto<R, ADS> p;
		for (unsigned i = 0; i < populationSize; i++)
			pMan.addSolution(p, *constructive.generateSolution(timelimit));

		return p;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << InitialPareto<R, ADS>::idComponent() << ":BasicInitialPareto";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class GRInitialPareto: public InitialPareto<R, ADS>
{
public:
	GRConstructive<R, ADS>& constructive;
	RandGen& rg;
	double maxAlpha; // limit the solution to be not so random
	paretoManager<R, ADS> pMan;

	GRInitialPareto(GRConstructive<R, ADS>& _constructive, RandGen& _rg, double _maxAlpha, MultiEvaluator<R, ADS>& _mev) :
			constructive(_constructive), rg(_rg), maxAlpha(_maxAlpha), pMan(paretoManager<R, ADS>(_mev))
	{
	}

	virtual ~GRInitialPareto()
	{
	}


	virtual Pareto<R, ADS>& generatePareto(unsigned populationSize, double timelimit = 100000000)
	{
		Timer tnow;

		Pareto<R, ADS>* p = new Pareto<R, ADS>;
		unsigned i = 0;
		while ((i < populationSize) && (tnow.now() < timelimit))
		{
			float alpha = rg.rand01();
			while (alpha > maxAlpha)
			{
				alpha = rg.rand01();
			}

			if (alpha == 0)
				alpha = 0.00001;

			pMan.addSolution(*p, constructive.generateGRSolution(alpha));
			i++;
		}
		return *p;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << InitialPareto<R, ADS>::idComponent() << ":GRInitialPareto";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_INITIALPARETO_H_*/
