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

#ifndef TSP2_EVALUATOR_HPP_
#define TSP2_EVALUATOR_HPP_

// #include <cmath>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>

#include "Evaluation.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

#define TSP_EPSILON 0.0001

namespace TSP2 {

// class MyEvaluator : public Evaluator<SolutionTSP>
class MyEvaluator : public Evaluator<SolutionTSP, EvaluationTSP> {
 private:
  ProblemInstance& pTSP;

 public:
  long long evaluations;

  using Evaluator<SolutionTSP>::evaluate;  // prevents name hiding

  MyEvaluator(ProblemInstance& _pTSP) : pTSP(_pTSP) { evaluations = 0; }

  Evaluation<> evaluate(const RepTSP& r, const OPTFRAME_DEFAULT_ADS*) override {
    evaluations++;
    double fo = 0;  // Evaluation<> Function Value

    for (int i = 0; i < ((int)r.size()) - 1; i++) {
      int j = r.at(i);
      int z = r.at(i + 1);

      double val = pTSP.dist(j, z);
      fo += val;
    }

    int k = r.at(((int)r.size()) - 1);
    int l = r.at(0);

    double val = pTSP.dist(k, l);
    fo += val;

    return Evaluation(fo);
  }

  virtual bool betterThan(double f1, double f2) {
    return (f1 < (f2 - TSP_EPSILON));
  }

  virtual bool isMinimization() const { return true; }

  virtual string id() const override {
    return "OptFrame:Evaluator:MyEvaluator";
  }

  void print() const override { std::cout << "TSP evaluation function" << std::endl; }
};

}  // namespace TSP2

#endif /*TSP2_EVALUATOR_HPP_*/
