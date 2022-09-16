// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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

#include "SearchOutput.hpp"

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
   //std::optional<XSH> best;
   // current/working XSH2 object: population/etc
   //std::optional<XSH2> incumbent;
   // ----------
   //
   // callback action 'onBest' is triggered after best is updated (if 'beforeUpdateBest' is required some day, think about it.. not now)
   // returning 'false' should lead to aborting execution (by target, for instance)
   //
   bool (*onBest)(GlobalSearch<XES, XSH, XES2, XSH2>& self, const XSH& best) =
     [](GlobalSearch<XES, XSH, XES2, XSH2>& self, const XSH& best) { return true; };
   //
   bool (*onIncumbent)(GlobalSearch<XES, XSH, XES2, XSH2>& self, const XSH2& incumbent) =
     [](GlobalSearch<XES, XSH, XES2, XSH2>& self, const XSH2& incumbent) { return true; };
   // strict or non-strict search
   bool strict{ true };

   GlobalSearch()
   {
   }

   virtual ~GlobalSearch()
   {
   }

   // Assuming method is not thread-safe. Now, we can easily use flag SearchStatus::RUNNING.
   virtual SearchOutput<XES, XSH> search(const StopCriteria<XEv>& stopCriteria) = 0;

   /*
   virtual SearchStatus searchBy(std::optional<XSH>& _best, std::optional<XSH2>& _inc, const StopCriteria<XEv>& stopCriteria)
   {
      best = _best;
      incumbent = _inc;
      return search(stopCriteria);
   }
*/

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
      ss << Component::idComponent() << ":GlobalSearch:";
      return ss.str();
   }

   virtual std::string toString() const override
   {
      return id();
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

   virtual std::string toString() const override
   {
      return id();
   }

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
