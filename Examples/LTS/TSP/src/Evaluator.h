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

#ifndef TSP_EVALUATOR_HPP_
#define TSP_EVALUATOR_HPP_

// #include <cmath>
//  C
#include <math.h>
#include <stdlib.h>
// C++
#include <string>
#include <utility>
//
#include <OptFrame/printable/printable.hpp>
using namespace optframe;
//
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Util/NSAdapter/NSSeqTSPSwap.hpp>

#include "Evaluation.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

#define TSP_EPSILON 0.0001

namespace TSP {

class TSPEvaluator : public Evaluator<SolutionTSP, EvaluationTSP> {
 private:
  sref<ProblemInstance> pI;

 public:
  bool Minimizing;

  vector<int> solutions;
  vector<long long> solNSTotal;
  vector<long long> solNSImp;
  int solMin;
  int solMax;

  bool doStats;

  // using Evaluator<SolutionTSP>::evaluate; // prevents name hiding

  TSPEvaluator(sref<ProblemInstance> pI)
      : Evaluator<SolutionTSP, EvaluationTSP>(new Minimization(),
                                              true /*allow costs*/),
        pI{pI} {
    Minimizing = true;
    solutions = vector<int>(50000, 0);
    solMax = 0;
    solMin = 2000000000;
    solNSTotal = vector<long long>(50000, 0);
    solNSImp = solNSTotal;

    doStats = true;
  }

  bool verify(const RepTSP& r) {
    assert(int(r.size()) == pI->n);
    vector<bool> count(pI->n, false);
    for (int i = 0; i < pI->n; i++) count[r[i]] = true;
    for (int i = 0; i < pI->n; i++) assert(count[i]);
    return true;
  }

  Evaluation<> evaluate(const SolutionTSP& s) override {
    const RepTSP& r = s.getR();
    double fo = 0;  // Evaluation<> Function Value

    for (int i = 0; i < ((int)r.size()) - 1; i++) {
      int j = r.at(i);
      int z = r.at(i + 1);

      double val = (*pI->dist)(j, z);
      fo += val;
    }

    int k = r.at(((int)r.size()) - 1);
    int l = r.at(0);

    double val = (*pI->dist)(k, l);
    fo += val;

    // fo = int(fo); // cast to int
    /*
      if (doStats)
      {
          int ifo = int(fo);

          if (ifo >= int(solutions.size()))
          {
              std::cout << "ERROR!!!! FO IS BIGGER THAN SOLUTION LIMIT!" << ifo
      << std::endl; exit(1);
          }
          solutions[ifo]++;

          if (ifo < solMin)
              solMin = ifo;
          if (ifo > solMax)
              solMax = ifo;

          // explore neighborhood
          int total = 0;
          int impSols = 0;

          exploreNeighborhood(r, total, impSols);
          if (total == 0)
          {
              std::cout << "ERRO! no neighbors!!" << std::endl;
              std::cout << r << std::endl;
              exit(1);
          }

          solNSTotal[ifo] += total;
          solNSImp[ifo] += impSols;
      }
      */

    return Evaluation<>(fo);
  }

  void exploreNeighborhood(RepTSP r, int& totalNeigh, int& improvingSols) {
    NSSeqTSPSwap<int> tspswap;

    // int ads;
    SolutionTSP s(r);  // TODO: think
    EvaluationTSP e;   // TODO: think
    ESolutionTSP se = make_pair(s, e);

    uptr<NSIterator<ESolutionTSP>> it = tspswap.getIterator(se);
    it->first();

    // std::cout << "got iterator: " << it.toString() << std::endl;

    CopySolution<RepTSP> base(r, nullptr);
    while (!it->isDone()) {
      // std::cout << "will get move" << std::endl;
      totalNeigh++;
      uptr<Move<ESolutionTSP>> m = it->current();
      // m.print();
      SolutionTSP s2(r);  // TODO: think
      EvaluationTSP e2;   // TODO: think
      ESolutionTSP se2 = make_pair(s2, e2);

      if (m->canBeApplied(se2)) {
        doStats = false;
        // Evaluation<>* e = nullptr; // dummy // TODO:
        EvaluationTSP e;  // dummy // TODO:
        pair<SolutionTSP, EvaluationTSP> pse(base, e);

        // MoveCost<>& mc = *this->moveCost(m, pse);
        op<Evaluation<>> mc = this->moveCost(*m, pse);
        doStats = true;
        // if (mc.getObjFunctionCost() < 0)
        if (mc->evaluation() < 0) improvingSols++;
        // delete &mc;
      }
      // delete &m;
      it->next();
    }
    // std::cout << "finished explore" << std::endl;
  }

  /*
     // TODO: remove this? // maybe only operate on XEv space?
   virtual bool betterThan(evtype f1, evtype f2)
   {
       if (Minimizing)
           return (f1 < (f2 - TSP_EPSILON));
       else
           return (f1 > (f2 + TSP_EPSILON));
   }
*/

  // override from Direction
  virtual inline bool betterThan(const Evaluation<double>& e1,
                                 const Evaluation<double>& e2) override {
    // std::cout << "betterThan!" << std::endl;
    // e1.print();
    // e2.print();
    //
    if (isMinimization())
      // return ::fabs(e2.evaluation() - e1.evaluation()) >= 0.00001;
      return (e2.evaluation() - e1.evaluation()) > 0.00001;
    else
      // return ::fabs(e1.evaluation() - e2.evaluation()) >= 0.00001;
      return (e1.evaluation() - e2.evaluation()) > 0.00001;
  }

  // override from Direction
  virtual bool isMinimization() const override { return Minimizing; }

  virtual string id() const override {
    string pai = Evaluator<SolutionTSP, EvaluationTSP>::idComponent();
    pai.append(":TSPEvaluator");
    return pai;
  }
};

// only evaluates by representation
class TSPRepEvaluator : public Evaluator<RepTSP, EvaluationTSP> {
 private:
  sref<ProblemInstance> pI;

 public:
  bool Minimizing;

  vector<int> solutions;
  vector<long long> solNSTotal;
  vector<long long> solNSImp;
  int solMin;
  int solMax;

  bool doStats;

  // using Evaluator<SolutionTSP>::evaluate; // prevents name hiding

  TSPRepEvaluator(sref<ProblemInstance> pI)
      : Evaluator<RepTSP, EvaluationTSP>(new Minimization(), true), pI{pI} {
    Minimizing = true;
    // this->pI = pI;
    solutions = vector<int>(50000, 0);
    solMax = 0;
    solMin = 2000000000;
    solNSTotal = vector<long long>(50000, 0);
    solNSImp = solNSTotal;

    doStats = true;
  }

  Evaluation<> evaluate(const RepTSP& r) override {
    double fo = 0;  // Evaluation<> Function Value

    for (int i = 0; i < ((int)r.size()) - 1; i++) {
      int j = r.at(i);
      int z = r.at(i + 1);

      double val = (*pI->dist)(j, z);
      fo += val;
    }

    int k = r.at(((int)r.size()) - 1);
    int l = r.at(0);

    double val = (*pI->dist)(k, l);
    fo += val;

    return Evaluation<>(fo);
  }

  // override from Direction
  virtual inline bool betterThan(const Evaluation<double>& e1,
                                 const Evaluation<double>& e2) override {
    // std::cout << "betterThan!" << std::endl;
    // e1.print();
    // e2.print();
    //
    if (Minimizing)
      // return ::fabs(e2.evaluation() - e1.evaluation()) >= 0.00001;
      return (e2.evaluation() - e1.evaluation()) > 0.00001;
    else
      // return ::fabs(e1.evaluation() - e2.evaluation()) >= 0.00001;
      return (e1.evaluation() - e2.evaluation()) > 0.00001;
  }

  // override from Direction
  virtual bool isMinimization() const override { return Minimizing; }

  virtual string id() const override {
    string pai = Evaluator<RepTSP, EvaluationTSP>::idComponent();
    pai.append(":TSPRepEvaluator");
    return pai;
  }
};

}  // namespace TSP

#endif /*TSP_EVALUATOR_HPP_*/
