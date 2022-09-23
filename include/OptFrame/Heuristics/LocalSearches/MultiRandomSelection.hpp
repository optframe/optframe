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

#ifndef OPTFRAME_MIRS_HPP_
#define OPTFRAME_MIRS_HPP_

// ==============================================================================================
// This is a Multi Improvement (MI) implementation
//
// The MI method was officially proposed in 2016 by Coelho et al (WAMCA/Los Angeles)
// "A Benchmark on Multi Improvement Neighborhood Search Strategies in CPU/GPU Systems"
//
// Method detailed in 2016 PhD thesis by Eyder Rios (pt-BR)
// "Exploração de Estratégias de Busca Local em Ambientes CPU/GPU"
//
// Dataflow extension in 2018 by Araujo et al
// "A DVND Local Search Implemented on a Dataflow Architecture for the Minimum Latency Problem"
//
// Heuristic MI extension to VRPs in 2019 by Silva Jr (pt-BR)
// "Multi Improvement: uma Solução Alternativa para o Problema de Roteamento de Veículos"
//
// Parallel GPU Data Flow extension in 2020 by Araujo et al
// "A multi-improvement local search using dataflow and GPU to solve the minimum latency problem"
//
// Exact dynamic programming approach in 2020 by Silva et al
// "Finding the Maximum Multi Improvement on neighborhood exploration"
// ==============================================================================================

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"

#include "../../Timer.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class MultiRandomSelection : public LocalSearch<XES, XEv>
{
private:
   sref<GeneralEvaluator<XES, XEv>> eval;
   sref<NS<XES, XEv, XSH>> ns;
   // number of moves for random sampling
   int countMoves;
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
   MultiRandomSelection(
     sref<GeneralEvaluator<XES, XEv>> _eval,
     sref<NS<XES, XEv, XSH>> _ns,
     int _countMoves,
     XEv _stopCost)
     : eval{ _eval }
     , ns{ _ns }
     , countMoves{ _countMoves }
     , stopCost{ _stopCost } // stopCost is typically 0 (or 0.0), for Sing.Obj.Problems
   {
      // only allow NS which is Solution Independent
      assert(ns->isSolutionIndependent());
      // only allow NS which explictly supports Move Independence
      assert(ns->supportsMoveIndependence());
   }

   virtual ~MultiRandomSelection()
   {
   }

   virtual SearchStatus searchFrom(XSH& se, const StopCriteria<XEv>& sosc) override
   {
      if (Component::information)
         std::cout << "MIRS::starts for " << ns->toString() << std::endl;

      // TODO: count must be positive
      assert(countMoves > 0);
      // no iterator here!

      // ===============
      // store all moves
      //
      if (Component::information)
         std::cout << "MIRS storing all moves" << std::endl;
      //
      // safety count of tries (prevents infinite loop)
      int tries = 5 * countMoves;
      //
      std::vector<Move<XES, XEv, XSH>*> allMoves;
      while (((int)allMoves.size()) < countMoves) {
         Move<XES, XEv, XSH>* m = ns->randomMove(se).release();
         if (m->canBeApplied(se))
            allMoves.push_back(m);
         else {
            delete m;
            tries--;
            if (tries == 0) {
               std::cout << "MIRS warning: not enough moves! aborting...";
               return SearchStatus::FAILED;
            }
         }
      }

      if (allMoves.size() == 0) {
         if (Component::warning)
            std::cout << "MIRS::warning: empty neighborhood" << std::endl;
         //
         return SearchStatus::NO_REPORT;
      }

      // =================
      // compute all costs
      //
      if (Component::information)
         std::cout << "MIRS computing all costs (" << allMoves.size() << ")" << std::endl;
      //
      std::vector<std::pair<XEv, int>> allCosts(allMoves.size());
      for (int k = 0; k < (int)allMoves.size(); k++)
         allCosts[k] = std::make_pair(eval->moveCost(*allMoves[k], se), k);

      // =================
      // invoke sorter
      //
      if (Component::information)
         std::cout << "MIRS invoking sorter" << std::endl;
      //
      this->sorter(allCosts);

      // =================
      // select and apply
      //
      if (Component::information)
         std::cout << "MIRS selecting and applying" << std::endl;
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
         std::cout << "MIRS count=" << countAcc << std::endl;
         std::cout << "MIRS accumulate=" << accumulate << std::endl;
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

   virtual bool compatible(string s) override
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

   virtual string toString() const override
   {
      stringstream ss;
      ss << "MIRS: " << ns->toString();
      return ss.str();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
class MultiRandomSelectionBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~MultiRandomSelectionBuilder()
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
      sptr<NS<XES, XEv, XSH>> nsseq;
      hf.assign(nsseq, *scanner.nextInt(), scanner.next()); // reads backwards!

      return new MultiRandomSelection<XES, XEv, XSH>(eval, nsseq);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
      params.push_back(make_pair(NS<XES, XEv, XSH>::idComponent(), "neighborhood structure"));

      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == MultiRandomSelection<XES, XEv>::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << LocalSearchBuilder<S, XEv>::idComponent() << ":MI";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif /*OPTFRAME_MIRS_HPP_*/
