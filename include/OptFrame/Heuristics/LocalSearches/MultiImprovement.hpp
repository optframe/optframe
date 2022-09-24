// OptFrame 4 - Optimization Framework
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

#ifndef OPTFRAME_MI_HPP_
#define OPTFRAME_MI_HPP_

// =====================================================================================================
// This is a Multi Improvement (MI) implementation
//
// The MI method was officially presented in 2016 by I.M. Coelho (Best Paper Award - WAMCA/Los Angeles)
// "A Benchmark on Multi Improvement Neighborhood Search Strategies in CPU/GPU Systems"
// Authors: Eyder Rios; Igor M. Coelho; Luiz Satoru Ochi; Cristina Boeres; Ricardo Farias
//
// Dataflow extension in 2018 by Araujo et al
// "A DVND Local Search Implemented on a Dataflow Architecture for the Minimum Latency Problem"
//
// Parallel GPU Data Flow extension in 2020 by Araujo et al
// "A multi-improvement local search using dataflow and GPU to solve the minimum latency problem"
//
// Exact dynamic programming approach in 2020 by Silva et al
// "Finding the Maximum Multi Improvement on neighborhood exploration"
// ======================================================================================================

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"

#include "../../Timer.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class MultiImprovement : public LocalSearch<XES, XEv>
{
private:
   sref<GeneralEvaluator<XES, XEv>> eval;
   sref<NSSeq<XES, XEv, XSH>> nsSeq;
   // cost "zero"
   XEv stopCost;
   //
   using objType = typename XEv::objType;
   //
public:
   std::function<void(std::vector<std::pair<XEv, int>>&)> sorter{
      [this](std::vector<std::pair<XEv, int>>& v) -> void {
         std::sort(
           v.begin(),
           v.end(),
           [this](const std::pair<XEv, int>& e1, const std::pair<XEv, int>& e2) -> bool {
              return this->eval->betterStrict(e1.first, e2.first);
           });
      }
   };

public:
   //
   MultiImprovement(
     sref<GeneralEvaluator<XES, XEv>> _eval,
     sref<NSSeq<XES, XEv, XSH>> _nsSeq,
     XEv _stopCost)
     : eval{ _eval }
     , nsSeq{ _nsSeq }
     , stopCost{ _stopCost } // stopCost is typically 0 (or 0.0), for Sing.Obj.Problems
   {
      // only allow NSSeq which is Solution Independent
      assert(nsSeq->isSolutionIndependent());
      // only allow NSSeq which explictly supports Move Independence
      assert(nsSeq->supportsMoveIndependence());
   }

   virtual ~MultiImprovement()
   {
   }

   virtual SearchStatus searchFrom(XSH& se, const StopCriteria<XEv>& sosc) override
   {
      if (Component::information)
         std::cout << "MI::starts for " << nsSeq->toString() << std::endl;

      // TODO: verify if it's not null
      uptr<NSIterator<XES, XEv, XSH>> it = nsSeq->getIterator(se);
      //
      if (!it) {
         if (Component::warning)
            std::cout << "MI::warning: empty iterator" << std::endl;
         return SearchStatus::FAILED;
      }
      //

      // ===============
      // store all moves
      //
      if (Component::information)
         std::cout << "MI storing all moves" << std::endl;
      //
      std::vector<Move<XES, XEv, XSH>*> allMoves;
      for (it->first(); !it->isDone(); it->next())
         allMoves.push_back(it->current().release());

      if (allMoves.size() == 0) {
         if (Component::warning)
            std::cout << "MI::warning: empty neighborhood" << std::endl;
         //
         return SearchStatus::NO_REPORT;
      }

      // =================
      // compute all costs
      //
      if (Component::information)
         std::cout << "MI computing all costs" << std::endl;
      //
      std::vector<std::pair<XEv, int>> allCosts(allMoves.size());
      for (int k = 0; k < (int)allMoves.size(); k++)
         if (allMoves[k]->canBeApplied(se))
            allCosts[k] = std::make_pair(eval->moveCost(*allMoves[k], se), k);
         else
            allCosts[k].second = -1; // flag as useless

      // =================
      // invoke sorter
      //
      if (Component::information)
         std::cout << "MI invoking sorter" << std::endl;
      //
      this->sorter(allCosts);

      // =================
      // select and apply
      //
      if (Component::information)
         std::cout << "MI selecting and applying" << std::endl;
      //
      objType accumulate;
      optframe::numeric_zero(accumulate);
      //
      int countAcc = 0;
      //
      for (unsigned k = 0; k < allCosts.size(); k++) {
         // check if move was excluded
         if (allCosts[k].second != -1)
            if (eval->betterStrict(allCosts[k].first, stopCost)) {
               countAcc++;
               accumulate += allCosts[k].first.getObjFunction();
               // ==========
               // apply move
               //
               allMoves[allCosts[k].second]->applyUpdate(se);
               eval->reevaluate(se); // updates 'e' part

               // ======================
               // find conflicting moves
               //
               for (unsigned c = k + 1; c < allCosts.size(); c++)
                  if (allCosts[c].second != -1)
                     if (!allMoves[allCosts[k].second]->independentOf(*allMoves[allCosts[c].second]))
                        allCosts[c].second = -1; // flag as conflicting
            }
      }

      if (Component::information) {
         std::cout << "MI count=" << countAcc << std::endl;
         std::cout << "MI accumulate=" << accumulate << std::endl;
      }

      // ==============
      // free resources
      //
      for (unsigned i = 0; i < allMoves.size(); i++)
         delete allMoves[i];
      allMoves.clear();

      // report 'all is well' (and dance...)
      return SearchStatus::NO_REPORT;
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearch<XES, XEv>::idComponent() << "MI";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << toString() << endl;
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << "MI: " << nsSeq->toString();
      return ss.str();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
class MultiImprovementBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~MultiImprovementBuilder()
   {
   }

   virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      if (!scanner.hasNext())
         return nullptr;
      sptr<GeneralEvaluator<XES, XEv>> eval;
      hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

      if (!scanner.hasNext())
         return nullptr;
      sptr<NSSeq<XES, XEv, XSH>> nsseq;
      hf.assign(nsseq, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new MultiImprovement<XES, XEv, XSH>(eval, nsseq);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(), "neighborhood structure"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == MultiImprovement<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":MI";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_MI_HPP_*/
