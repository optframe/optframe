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

#include <cstring>
#include <iostream>
#include <vector>

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "BaseConcepts.hpp"
#include "InitialSearch.hpp"
#include "StopCriteria.hpp"

using namespace std;

namespace optframe {

// Defaulting SearchSpace to XES, it means, <S,XEv> space (typically, single obj search)
//template<XESolution XES, XSearch<XES> XSH = XES, XSearchMethod XM = Component>
//
//template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES>
//
// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
template<XESolution XES, XSearch<XES> XSH = XES, XESolution XES2 = XES, XSearch<XES2> XSH2 = XES2>
class GlobalSearch : public Component
{
   using XEv = typename XES::second_type;

public:
   // best known XSH object: solution/pareto/etc
   std::optional<XSH> best;
   // current/working XSH2 object: population/etc
   std::optional<XSH2> incumbent;
   // ----------
   //
   // callback action 'onBest' is triggered after best is updated (if 'beforeUpdateBest' is required some day, think about it.. not now)
   // returning 'false' should lead to aborting execution (by target, for instance)
   //
   bool (*onBest)(GlobalSearch<XES, XSH, XES2, XSH2>& self) = [](GlobalSearch<XES, XSH, XES2, XSH2>& self) { return true; };
   bool (*onIncumbent)(GlobalSearch<XES, XSH, XES2, XSH2>& self) = [](GlobalSearch<XES, XSH, XES2, XSH2>& self) { return true; };
   // strict or non-strict search
   bool strict{ true };

   GlobalSearch()
   {
   }

   virtual ~GlobalSearch()
   {
   }

   // Assuming method is not thread-safe. Now, we can easily use flag SearchStatus::RUNNING.
   virtual SearchStatus search(const StopCriteria<XEv>& stopCriteria) = 0;

   virtual SearchStatus searchBy(std::optional<XSH>& _best, std::optional<XSH2>& _inc, const StopCriteria<XEv>& stopCriteria)
   {
      best = _best;
      incumbent = _inc;
      return search(stopCriteria);
   }

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

template<XSolution S, XEvaluation XEv, XESolution XES, XSearch<XES> XSH, XESolution XES2, XSearch<XES> XSH2 = XSH>
class GlobalSearchBuilder : public ComponentBuilder<S, XEv, XSH>
{
public:
   virtual ~GlobalSearchBuilder()
   {
   }

   virtual GlobalSearch<XES, XSH, XES2, XSH2>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, XSH>& hf, string family = "") = 0;

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, XSH>& hf, string family = "")
   {
      return build(scanner, hf, family);
   }

   virtual vector<pair<string, string>> parameters() = 0;

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
