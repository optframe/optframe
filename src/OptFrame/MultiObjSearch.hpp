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

#include "SingleObjSearch.hpp" // TODO: remove

#include "GlobalSearch.hpp" // Base class

namespace optframe
{

/*
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
*/


// This MultiObjSearch perspective inherits from Multi Solution Search, 
// considering a X2ES space with Pareto structure

template<XSolution S, XEvaluation XMEv = Evaluation<>, XESolution XMES = pair<S, XMEv>>
//template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class MultiObjSearch: public GlobalSearch<XMES, XMEv, Pareto<S, XMEv, XMES>> // public Component
{
   using XSH = Pareto<S, XMEv, XMES>; // search space
public:

	MultiObjSearch()
	{
	}

	virtual ~MultiObjSearch()
	{
	}

	//virtual Pareto<S, XMEv>* search(MOSC& stopCriteria, Pareto<S, XMEv>* _pf = nullptr) = 0;
   //
   //virtual SearchStatus search(std::optional<Pareto<S, XMEv>>& p, const StopCriteria<XMEv>& stopCriteria) = 0;
   virtual SearchStatus search(const StopCriteria<XMEv>& stopCriteria) = 0;

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

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class MultiObjSearchBuilder: public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~MultiObjSearchBuilder()
	{
	}

	virtual MultiObjSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_MULTI_OBJ_SEARCH_HPP_ */
