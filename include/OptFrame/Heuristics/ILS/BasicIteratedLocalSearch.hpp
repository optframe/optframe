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

#ifndef OPTFRAME_BASICILS_HPP_
#define OPTFRAME_BASICILS_HPP_

#include <math.h>
#include <vector>

#include "../../LocalSearch.hpp"

#include "BasicILSPerturbation.hpp"
#include "IteratedLocalSearch.hpp"

#include "ILS.h"

namespace optframe {

typedef int BasicHistory;

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicIteratedLocalSearch : public IteratedLocalSearch<BasicHistory, XES, XEv>
{
protected:
   sref<LocalSearch<XES, XEv>> ls;
   sref<BasicILSPerturbation<XES, XEv>> p;
   int iterMax;

public:
   //BasicIteratedLocalSearch(Evaluator<XES, XEv>& e, Constructive<S>& constructive, LocalSearch<XES, XEv>& _ls, BasicILSPerturbation<S, XEv>& _p, int _iterMax) :
   BasicIteratedLocalSearch(sref<GeneralEvaluator<XES, XEv>> e, sref<InitialSearch<XES, XEv>> constructive, sref<LocalSearch<XES, XEv>> _ls, sref<BasicILSPerturbation<XES, XEv>> _p, int _iterMax)
     : IteratedLocalSearch<BasicHistory, XES, XEv>(e, constructive)
     , ls(_ls)
     , p(_p)
     , iterMax(_iterMax)
   {
   }

   virtual ~BasicIteratedLocalSearch()
   {
   }

   virtual sref<BasicHistory> initializeHistory() override
   {
      sref<int> iter = new int;
      iter = 0;

      return iter;
   }

   virtual void localSearch(XES& se, const StopCriteria<XEv>& sosc) override
   {
      ls->searchFrom(se, sosc);
   }

   virtual void perturbation(XES& se, const StopCriteria<XEv>& sosc, sref<BasicHistory> history) override
   {
      int iter = history;

      p->perturb(se, sosc);

      // Incrementa a iteracao
      iter++;

      // Atualiza o historico
      history = iter;
   }

   virtual bool acceptanceCriterion(const Evaluation<>& e1, const Evaluation<>& e2, sref<BasicHistory> history) override
   {
      //if (IteratedLocalSearch<BasicHistory, XES, XEv>::evaluator.betterThan(e1, e2))
      //if (e1.betterStrict(e2))
      if (IteratedLocalSearch<BasicHistory, XES, XEv>::evaluator->betterStrict(e1, e2)) {
         // =======================
         //   Melhor solucao: 's2'
         // =======================
         cout << "Best fo: " << e1.evaluation();
         cout << " on [iter " << history << "]" << endl;

         // =======================
         //  Atualiza o historico
         // =======================
         // iter = 0
         history = 0;

         // =======================
         //    Retorna s2
         // =======================
         return true;
      } else
         return false;
   }

   virtual bool terminationCondition(sref<BasicHistory> history) override
   {
      int iter = history;

      return (iter >= iterMax);
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
   }

   virtual string id() const override
   {
      return idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << IteratedLocalSearch<BasicHistory, XES, XEv>::idComponent() << "BasicILS";
      return ss.str();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicIteratedLocalSearchBuilder : public ILS
  , public SingleObjSearchBuilder<S, XEv, XES>
{
public:
   virtual ~BasicIteratedLocalSearchBuilder()
   {
   }

   virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") override
   {
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      //Constructive<S>* constructive;
      sptr<InitialSearch<XES, XEv>> constructive;
      hf.assign(constructive, *scanner.nextInt(), scanner.next()); // reads backwards!

      string rest = scanner.rest();

      pair<sptr<LocalSearch<XES, XEv>>, std::string> method;
      method = hf.createLocalSearch(rest);

      sptr<LocalSearch<XES, XEv>> h = method.first;

      scanner = Scanner(method.second);

      sptr<BasicILSPerturbation<XES, XEv>> pert;
      hf.assign(pert, *scanner.nextInt(), scanner.next()); // reads backwards!

      int iterMax = *scanner.nextInt();

      return new BasicIteratedLocalSearch<XES, XEv>(eval, constructive, h, pert, iterMax);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      //params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(InitialSearch<XES, XEv>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));
      params.push_back(make_pair(BasicILSPerturbation<XES, XEv>::idComponent(), "ils perturbation"));
      params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == BasicIteratedLocalSearch<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":" << ILS::family() << "BasicILS";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_BASICILS_HPP_*/
