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

#ifndef OPTFRAME_GLOBAL_SEARCH_HPP_
#define OPTFRAME_GLOBAL_SEARCH_HPP_


#include <iostream>
#include <vector>
#include <cstring>

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "BaseConcepts.hpp"
#include "StopCriteria.hpp"
#include "InitialSearch.hpp"

using namespace std;

namespace optframe
{

// Defaulting SearchSpace to XES, it means, <S,XEv> space (typically, single obj search)
template<XESolution XES, XSearch<XES> XSH = XES, XSearchMethod XM = Component>
class GlobalSearch: public Component
{
public:

	GlobalSearch()
	{
	}

	virtual ~GlobalSearch()
	{
	}

   virtual SearchStatus search(std::optional<XSH>& p, const StopCriteria<XSH, XM>& stopCriteria) = 0;

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
		ss << Component::idComponent() << "GlobalSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<XSolution S, XEvaluation XEv, XESolution XES, XSearch<XES> XSH>
class GlobalSearchBuilder: public ComponentBuilder<S, XEv, XSH>
{
public:
	virtual ~GlobalSearchBuilder()
	{
	}

	virtual GlobalSearch<XES, XSH>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, XSH>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, XSH>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, XSH>::idComponent() << "GlobalSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

} // namespace optframe

#endif /* OPTFRAME_GLOBAL_SEARCH_HPP_ */
