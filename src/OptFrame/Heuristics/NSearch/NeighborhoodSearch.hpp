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

#ifndef OPTFRAME_NEIGHBORHOOD_SEARCH_HPP_
#define OPTFRAME_NEIGHBORHOOD_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "../../LocalSearch.hpp"
#include "../../NeighborhoodExploration.hpp"

namespace optframe {

// TODO: may pass just XESolution and XEvaluation here (for StopCriteria)... no XSolution explicitly required.
template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES> // defaults to XSH = XES
class NeighborhoodSearch : public LocalSearch<XES, XEv, XSH>
{

public:
   NeighborhoodExploration<XES, XEv>& nex;
   GeneralEvaluator<XES, XEv>& eval;

   NeighborhoodSearch(NeighborhoodExploration<XES, XEv>& _nex)
     : nex(_nex)
   {
   }

   virtual ~NeighborhoodSearch()
   {
   }

   virtual void searchFrom(XES& se, const StopCriteria<XEv>& stopCriteria)
   {
      op<MoveWithCost> movec = nex.searchMove(se, stopCriteria);
      //
      if (!movec)
         return;
      //
      // accept if it's improving
      if (eval.isStrictImprovement(movec)->second) {
         // apply move to solution
         movec->first->apply(se);
         // update cost
         movec->second.update(se.second);
      }
      // finished search
      // =====================================
      // should we loop as default?
      // should we stop only on local optimum?
      // =====================================
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":NeighborhoodSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>, XSearch<XES> XSH = XES>
class NeighborhoodSearchBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~NeighborhoodSearchBuilder()
   {
   }

   virtual NeighborhoodSearch<XES, XEv, XSH>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      return build(scanner, hf, family);
   }

   virtual vector<pair<string, string>> parameters() = 0;

   virtual bool canBuild(string) = 0;

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "NeighborhoodSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

} // namespace optframe

#endif /* OPTFRAME_NEIGHBORHOOD_SEARCH_HPP_ */
