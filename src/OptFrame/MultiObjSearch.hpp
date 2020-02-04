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

#ifndef OPTFRAME_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_MULTI_OBJ_SEARCH_HPP_

#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

#include "Solution.hpp"
#include "Population.hpp"
#include "Evaluation.hpp"
#include "Direction.hpp"

#include "MultiEvaluator.hpp"
#include "MultiEvaluation.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "Pareto.hpp"
#include "ParetoDominance.hpp"
#include "ParetoDominanceWeak.hpp"

namespace optframe
{

// Multi Objective Stopping Criteria
// Must include GENERAL stopping criteria
// specific stopping criteria for metaheuristics can be included in their constructors
class MOSC: public Component
{
public:
	// maximum timelimit (seconds)
	double timelimit;
	//Hypervolume indicator
	//	double hv;
	//Pareto cardinality
	//	double cardinality;

	MOSC(double _timelimit = 100000000.0) :
			timelimit(_timelimit)
	{
	}

	virtual ~MOSC()
	{
	}

	virtual string id() const
	{
		return "MOSC";
	}
};

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class MultiObjSearch: public Component
{
public:

	MultiObjSearch()
	{
	}

	virtual ~MultiObjSearch()
	{
	}

	virtual Pareto<R, ADS, S>* search(MOSC& stopCriteria, Pareto<R, ADS, S>* _pf = nullptr) = 0;

	virtual string log() const
	{
		return "Empty heuristic log.";
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class MultiObjSearchBuilder: public ComponentBuilder<R, ADS, S>
{
public:
	virtual ~MultiObjSearchBuilder()
	{
	}

	virtual MultiObjSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, S>::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_MULTI_OBJ_SEARCH_HPP_ */
