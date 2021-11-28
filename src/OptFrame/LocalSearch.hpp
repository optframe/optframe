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

#ifndef OPTFRAME_LOCAL_SEARCH_HPP_
#define OPTFRAME_LOCAL_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "Evaluation.hpp"
#include "Solution.hpp"

#include "Component.hpp"
#include "NSIterator.hpp"
#include "SingleObjSearch.hpp"

namespace optframe {

// TODO: may pass just XESolution and XEvaluation here (for StopCriteria)... no XSolution explicitly required.
template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES> // defaults to XSH = XES
class LocalSearch : public Component
{
   typedef vector<XEv*> FitnessValues;
   typedef const vector<const XEv*> ConstFitnessValues;

public:
   LocalSearch()
   {
      // DEFAULT for LocalSearch: silent
      Component::setSilent();
   }

   virtual ~LocalSearch()
   {
   }

   // core methods

   // copy-based version (TODO: deprecate this?)
   XES lsearch(const XES& se, const StopCriteria<XEv>& stopCriteria)
   {
      //S& s2 = s.clone();
      //XEv& e2 = e.clone();
      XES p2 = se; // implicit 'clone' here ??
      searchFrom(p2, stopCriteria);
      //return *new pair<S&, XEv&> (s2, e2);
      return p2;
   }

   // core methods

   // 1
   //virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria) = 0;

   // keeping only this method, for simplification
   // 2
   //virtual void exec(pair<S, XEv>& se, const StopCriteria<XEv>& stopCriteria) = 0;
   virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stopCriteria) = 0;

   // optional: set local optimum status (LOS)
   virtual void setLOS(LOS los, string nsid, XES& se)
   {
   }

   // optional: get local optimum status (LOS)
   virtual LOS getLOS(string nsid, XES& se)
   {
      return los_unknown;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":LocalSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool setVerboseR() override
   {
      this->setVerbose();
      return true;
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = XES>
class LocalSearchBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~LocalSearchBuilder()
   {
   }

   virtual LocalSearch<XES, XEv, XSH>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

   virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      return build(scanner, hf, family);
   }

   virtual vector<pair<string, string>> parameters() = 0;

   virtual bool canBuild(string) = 0;

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "LocalSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /* OPTFRAME_LOCAL_SEARCH_HPP_ */
