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

#ifndef OPTFRAME_MULTI_SOLUTION_SEARCH_HPP_
#define OPTFRAME_MULTI_SOLUTION_SEARCH_HPP_

// Search that happens using MultiSolution space (2^S)
// It could be MO Pareto Search, or populational search...

#include <iostream>
#include <vector>
#include <cstring>

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

using namespace std;

namespace optframe
{

//template<XSolution S, XEvaluation XEv = Evaluation<>>
template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class MultiSolutionSearch: public Component
{
public:

	MultiSolutionSearch()
	{
	}

	virtual ~MultiSolutionSearch()
	{
	}

	//virtual Pareto<S, XEv>* search(MOSC& stopCriteria, Pareto<S, XEv>* _pf = nullptr) = 0;
   virtual SearchStatus search(std::optional<X2ES>& p, SOSC& stopCriteria) = 0;

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
		ss << Component::idComponent() << "MultiSolutionSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class MultiSolutionSearchBuilder: public ComponentBuilder<S, XEv, X2ES>
{
public:
	virtual ~MultiObjSearchBuilder()
	{
	}

	virtual MultiObjSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, X2ES>::idComponent() << "MultiObjSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_MULTI_SOLUTION_SEARCH_HPP_ */
