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

template<XSolution S, XEvaluation XEv=Evaluation<>>
class SimpleLocalSearch : public SingleObjSearch<S, XEv>
{
protected:
   Evaluator<S, XEv>& evaluator;
   Constructive<S>& constructive;
   LocalSearch<S, XEv>& localSearch;

public:
   SimpleLocalSearch(Evaluator<S, XEv>& _evaluator, Constructive<S>& _constructive, LocalSearch<S, XEv>& _localSearch)
     : evaluator(_evaluator)
     , constructive(_constructive)
     , localSearch(_localSearch)
   {
   }

   virtual ~SimpleLocalSearch()
   {
   }

   //pair<S, Evaluation<>>* search(SOSC& sosc, const S* _s = nullptr, const Evaluation<>* _e = nullptr) override
   virtual std::optional<pair<S, XEv>> search(SOSC& sosc, const std::optional<pair<S, XEv>> input = std::nullopt) override
   {
      //cout << "SimpleLocalSearch search(" << target_f << "," << timelimit << ")" << endl;

      Timer tnow;

      std::optional<S> s = constructive.generateSolution(sosc.timelimit);
      if(!s)
         return std::nullopt; // nothing to return
      Evaluation<> e = evaluator.evaluate(*s);

      ////pair<S&, Evaluation<>&>& p = localSearch.search(s, e, sosc);

      //delete &s;

      return make_optional(make_pair(*s, e));
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (SingleObjSearch<S, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearch<S, XEv>::idComponent() << "SimpleLocalSearch";
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

template<XSolution S, XEvaluation XEv = Evaluation<>>
class SimpleLocalSearchBuilder : public SingleObjSearchBuilder<S, XEv>
{
public:
   virtual ~SimpleLocalSearchBuilder()
   {
   }

   virtual SingleObjSearch<S, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv>& hf, string family = "") override
   {
      Evaluator<S, XEv>* eval;
      hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

      Constructive<S>* constructive;
      hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

      string rest = scanner.rest();

      pair<LocalSearch<S, XEv>*, std::string> method;
      method = hf.createLocalSearch(rest);

      LocalSearch<S, XEv>* h = method.first;

      scanner = Scanner(method.second);

      return new SimpleLocalSearch<S, XEv>(*eval, *constructive, *h);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(LocalSearch<S, XEv>::idComponent(), "local search"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == SimpleLocalSearch<S, XEv>::idComponent();
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
