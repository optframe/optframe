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

#ifndef OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_
#define OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "Solution.hpp"
#include "Evaluation.hpp"

#include "Component.hpp"
#include "NSIterator.hpp"
#include "SingleObjSearch.hpp"

namespace optframe
{

// This is  NEx: Neighborhood Exploration

template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES>
using MoveWithCost = pair< uptr< Move<XES, XEv> >, XEv >;

template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES> // defaults to XSH = XES
class NeighborhoodExploration: public Component
{
   
public:

   NeighborhoodExploration()
   {
   }

   virtual ~NeighborhoodExploration()
   {
   }

   // Output move may be nullptr. Otherwise it's a pair of Move and its Cost.
   virtual op< MoveWithCost<XES, XEv> > searchMove(const XES& se, const StopCriteria<XEv>& stopCriteria) = 0;

   virtual bool compatible(string s)
   {
	   return ( s == idComponent() ) || ( Component::compatible(s) );
   }

   static string idComponent()
   {
	   stringstream ss;
	   ss << Component::idComponent() << ":NEx";
	   return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>, XSearch<XES> XSH = XES>
class NeighborhoodExplorationBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~NeighborhoodExplorationBuilder()
	{
	}

	virtual NeighborhoodExploration<XES, XEv, XSH>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "NEx";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}


#endif /* OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_ */
