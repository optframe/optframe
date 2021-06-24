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

#ifndef OPTFRAME_BASIC_VNS_HPP_
#define OPTFRAME_BASIC_VNS_HPP_

#include <math.h>
#include <vector>

#include "../../HeuristicFactory.hpp"
#include "../LocalSearches/BestImprovement.hpp"
#include "VNS.h"
#include "VariableNeighborhoodSearch.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicVNS : public VariableNeighborhoodSearch<XES, XEv>
{
public:
   typedef VariableNeighborhoodSearch<XES, XEv> super;

   //BasicVNS(Evaluator<XES>& evaluator, Constructive<S>& constructive, vector<NS<XES, XEv>*> vshake, vector<NSSeq<S>*> vsearch) :
   BasicVNS(sref<GeneralEvaluator<XES>> evaluator, sref<InitialSearch<XES, XEv>> constructive, vsref<NS<XES, XEv>> vshake, vsref<NSSeq<XES>> vsearch)
     : VariableNeighborhoodSearch<XES, XEv>(evaluator, constructive, vshake, vsearch)
   {
   }

   virtual ~BasicVNS()
   {
   }

   virtual LocalSearch<XES, XEv>& buildSearch(unsigned k_search)
   {
      return *new BestImprovement<XES, XEv>(super::evaluator, super::vsearch.at(k_search));
   }

   virtual string id() const
   {
      return idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << VariableNeighborhoodSearch<XES, XEv>::idComponent() << "BVNS";
      return ss.str();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicVNSBuilder : public ILS
  , public SingleObjSearchBuilder<S, XEv, XES>
{
public:
   virtual ~BasicVNSBuilder()
   {
   }

   virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      //Constructive<S>* constructive;
      sptr<InitialSearch<XES, XEv>> constructive;
      hf.assign(constructive, *scanner.nextInt(), scanner.next()); // reads backwards!

      vsptr<NS<XES, XEv>> _shakelist;
      hf.assignList(_shakelist, *scanner.nextInt(), scanner.next()); // reads backwards!
      vsref<NS<XES, XEv>> shakelist;
      for (auto x : _shakelist)
         shakelist.push_back(x);

      vsptr<NSSeq<XES, XEv>> _searchlist;
      hf.assignList(_searchlist, *scanner.nextInt(), scanner.next()); // reads backwards!
      vsref<NSSeq<XES, XEv>> searchlist;
      for (auto x : _searchlist)
         searchlist.push_back(x);

      return new BasicVNS<XES, XEv>(eval, constructive, shakelist, searchlist);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
      //params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(InitialSearch<XES, XEv>::idComponent(), "constructive heuristic"));

      stringstream ss;
      ss << NS<XES, XEv>::idComponent() << "[]";
      params.push_back(make_pair(ss.str(), "list of NS"));

      stringstream ss2;
      ss2 << NSSeq<XES, XEv>::idComponent() << "[]";
      params.push_back(make_pair(ss2.str(), "list of NSSeq"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == BasicVNS<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearchBuilder<S, XEv>::idComponent() << VNS::family() << "BasicVNS";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_BasicVNS_HPP_*/
