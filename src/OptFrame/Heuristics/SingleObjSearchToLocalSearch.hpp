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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_TO_LOCAL_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_TO_LOCAL_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../Constructive.hpp"
#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"

namespace optframe {

template<Representation R, Structure ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class SingleObjSearchToLocalSearch : public LocalSearch<R, ADS, S>
{
protected:
   Evaluator<R, ADS, S>& evaluator;
   SingleObjSearch<R, ADS, S>& sios;

public:
   SingleObjSearchToLocalSearch(Evaluator<R, ADS, S>& _evaluator, SingleObjSearch<R, ADS, S>& _sios)
     : evaluator(_evaluator)
     , sios(_sios)
   {
   }

   virtual ~SingleObjSearchToLocalSearch()
   {
   }

   virtual void exec(S& s, SOSC& sosc) override
   {
      Evaluation<> e = evaluator.evaluateSolution(s);

      exec(s, e, sosc);
   }

   virtual void exec(S& s, Evaluation<>& e, SOSC& sosc) override
   {
      pair<S, Evaluation<>>* r = sios.search(sosc, &s, &e);

      if (r) {
         s = r->first;
         e = r->second;
         delete r;
      }
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<R, ADS, S>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearch<R, ADS, S>::idComponent() << "SingleObjSearchToLocalSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << "SingleObjSearchToLocalSearch with sios: ";
      sios.print();
   }
};

template<Representation R, Structure ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class SingleObjSearchToLocalSearchBuilder : public LocalSearchBuilder<R, ADS, S>
{
public:
   virtual ~SingleObjSearchToLocalSearchBuilder()
   {
   }

   virtual LocalSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
   {
      Evaluator<R, ADS, S>* eval;
      hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

      string rest = scanner.rest();

      pair<SingleObjSearch<R, ADS, S>*, std::string> method;
      method = hf.createSingleObjSearch(rest);

      SingleObjSearch<R, ADS, S>* h = method.first;

      scanner = Scanner(method.second);

      return new SingleObjSearchToLocalSearch<R, ADS, S>(*eval, *h);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(Evaluator<R, ADS, S>::idComponent(), "evaluation function"));
      params.push_back(make_pair(SingleObjSearch<R, ADS, S>::idComponent(), "single obj search"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == SingleObjSearchToLocalSearch<R, ADS, S>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<R, ADS, S>::idComponent() << "SingleObjSearchToLocalSearch";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif
