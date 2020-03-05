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

#ifndef OPTFRAME_COMPONENT_BUILDER_H_
#define OPTFRAME_COMPONENT_BUILDER_H_

#include "Component.hpp"
#include "Solution.hpp"
#include "Solutions/CopySolution.hpp"
#include "MultiESolution.hpp"
#include "Evaluation.hpp"

#include "Scanner++/Scanner.h"

using namespace scannerpp;

namespace optframe
{

template<XSolution S, XEvaluation XEv, XESolution XES, X2ESolution<XES> X2ES> class HeuristicFactory;


// TODO: maybe use CRTP pattern! (at least from LocalSearch and SingleObjSearch)
template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class ComponentBuilder : public Component
{
public:
	virtual ~ComponentBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":ComponentBuilder:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_COMPONENT_BUILDER_H_*/
