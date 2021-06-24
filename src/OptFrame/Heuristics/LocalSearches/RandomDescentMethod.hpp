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

#ifndef OPTFRAME_RDM_HPP_
#define OPTFRAME_RDM_HPP_

#include "../../LocalSearch.hpp"

#include "../../NS.hpp"
#include "../../Timer.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class RandomDescentMethod : public LocalSearch<XES, XEv, XES>
{
private:
   sref<GeneralEvaluator<XES, XEv>> evaluator;
   sref<NS<XES, XEv>> ns;
   unsigned int iterMax;

public:
   RandomDescentMethod(sref<GeneralEvaluator<XES, XEv>> _eval, sref<NS<XES, XEv>> _ns, unsigned int _iterMax)
     : evaluator(_eval)
     , ns(_ns)
     , iterMax(_iterMax)
   {
   }

   virtual ~RandomDescentMethod()
   {
   }

   // DEPRECATED
   //virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
   //{
   //	Evaluation<> e = std::move(ev.evaluate(s));
   //	exec(s, e, stopCriteria);
   //}

   virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stopCriteria) override
   {
      //XSolution& s = se.first;
      //XEv& e = se.second;
      Timer tNow;

      unsigned int iter = 0;

      // TODO: de-referentiation of 'target_f' WILL crash, if not provided!! removing 'target_f'
      while ((iter < iterMax) && (tNow.now() < stopCriteria.timelimit)) //&& (evaluator.betterThan(*stopCriteria.target_f, se)))
      {
         //uptr<Move<XES, XEv>> move = ns.randomMove(s);
         uptr<Move<XES, XEv>> move = ns->randomMove(se);

         op<Evaluation<>> cost = nullopt;

         //if (move && move->canBeApplied(s))
         if (move && move->canBeApplied(se)) {
            cost = evaluator->moveCost(*move, se);
         } else {
            iter++;
            continue;
         }

         iter++;

         //if (cost && evaluator.isImprovement(*cost))
         //if (cost && cost->isImprovingStrict())
         if (cost && evaluator->isStrictImprovement(*cost)) {
            move->applyUpdate(se);
            evaluator->reevaluate(se);
            iter = 0;
         }
      }
      return SearchStatus::NO_REPORT;
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<XES, XEv>::idComponent() << ":RDM";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class RandomDescentMethodBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~RandomDescentMethodBuilder()
   {
   }

   virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      sptr<NS<XES, XEv>> ns;
      hf.assign(ns, *scanner.nextInt(), scanner.next()); // reads backwards!

      int iterMax = *scanner.nextInt();

      return new RandomDescentMethod<XES, XEv>(eval, ns, iterMax);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NS<XES, XEv>::idComponent(), "neighborhood structure"));
      params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == RandomDescentMethod<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":RDM";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_RDM_HPP_*/
