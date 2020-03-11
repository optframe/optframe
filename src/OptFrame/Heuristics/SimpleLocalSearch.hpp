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

#ifndef OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_
#define OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../Constructive.hpp"
#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv=Evaluation<>>
class SimpleLocalSearch : public SingleObjSearch<XES>
{
protected:
   Evaluator<XES, XEv>& evaluator;
   InitialSearch<XES, XEv>& constructive;
   LocalSearch<XES, XEv>& localSearch;

public:
   SimpleLocalSearch(Evaluator<XES, XEv>& _evaluator, InitialSearch<XES, XEv>& _constructive, LocalSearch<XES, XEv>& _localSearch)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , localSearch(_localSearch)
   {
   }

   virtual ~SimpleLocalSearch()
   {
   }

   //pair<S, Evaluation<>>* search(StopCriteria<XEv>& sosc, const S* _s = nullptr, const Evaluation<>* _e = nullptr) override
   //virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& sosc) override
   SearchStatus search(op<XES>& star, const StopCriteria<XEv>& sosc) override
   {
      //cout << "SimpleLocalSearch search(" << target_f << "," << timelimit << ")" << endl;

      Timer tnow;

      //std::optional<S> s = constructive.generateSolution(sosc.timelimit);
      std::optional<XES> pse = constructive.initialSearch(sosc);
      if(!pse)
         return SearchStatus::NO_NEWSOL; // nothing to return
      //Evaluation<> e = evaluator.evaluate(*s);

      ////pair<S&, Evaluation<>&>& p = localSearch.search(s, e, sosc);

      //delete &s;

      //return make_optional(make_pair(*s, e));
      //star = make_optional(make_pair(*s, e));
      star = make_optional(*pse);
      return SearchStatus::VALID_SOL;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearch<XES>::idComponent() << "SimpleLocalSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << "SimpleLocalSearch with:" << endl;
      cout << "constructive: ";
      constructive.print();
      cout << "local search: ";
      localSearch.print();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class SimpleLocalSearchBuilder : public SingleObjSearchBuilder<S, XEv, XES>
{
public:
   virtual ~SimpleLocalSearchBuilder()
   {
   }

   virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") override
   {
      Evaluator<XES, XEv>* eval;
      hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

      //Constructive<S>* constructive;
      InitialSearch<XES, XEv>* constructive;
      hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

      string rest = scanner.rest();

      pair<LocalSearch<XES, XEv>*, std::string> method;
      method = hf.createLocalSearch(rest);

      LocalSearch<XES, XEv>* h = method.first;

      scanner = Scanner(method.second);

      return new SimpleLocalSearch<XES, XEv>(*eval, *constructive, *h);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
      //params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(InitialSearch<XES, XEv>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == SimpleLocalSearch<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearchBuilder<S, XEv>::idComponent() << "SimpleLocalSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /*OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_*/
