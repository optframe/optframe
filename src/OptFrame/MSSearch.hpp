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

// ===================================================
// Search that happens using MultiSolution space (2^S)
// ===================================================
// It could be MO Pareto Search, or any populational search, being it single or multi objective.
// The idea is that MS management (Population or Pareto) is held by X2ES class itself, thus
//   reducing the burden of Evaluation objects to keep track of it.
// This finally allows to remove ParetoManager class.
// ===================================================

// THIS CLASS COULD PERHAPS BE DEPRECATED IN FAVOR OF GlobalSearch... NOT MUCH USE HERE, DOES IT?

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


template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class MSSearch: public Component
{
public:

	MSSearch()
	{
	}

	virtual ~MSSearch()
	{
	}

   // this should be pure virtual. useful for populational searches (single or multiobj) and general multiobj searches (by pareto sets)
   virtual SearchStatus search(std::optional<X2ES>& p, const StopCriteria<XSH>& stopCriteria) = 0;

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
		ss << Component::idComponent() << "MSSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class MSSearchBuilder: public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~MSSearchBuilder()
	{
	}

	virtual MSSearch<S, XEv, X2ES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "MSSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

} // namespace optframe

#endif /* OPTFRAME_MULTI_SOLUTION_SEARCH_HPP_ */
