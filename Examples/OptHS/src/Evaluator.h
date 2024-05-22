// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OptHS_EVALUATOR_HPP_
#define OptHS_EVALUATOR_HPP_

//#include <cmath>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>

#include "Evaluation.h"
#include "OptFrame/Direction.hpp"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

#define OptHS_EPSILON 0.0001

using namespace optframe;

namespace OptHS {

class OptHSEvaluator
    : public Evaluator<SolutionOptHS, EvaluationOptHS, ESolutionOptHS> {
 private:
  ProblemInstance& p;

 public:
  using Evaluator<SolutionOptHS,
                  EvaluationOptHS>::evaluate;  // prevents name hiding

  explicit OptHSEvaluator(ProblemInstance& _p)
      : Evaluator(new Minimization<EvaluationOptHS>()), p{_p} {}

  Evaluation<> evaluate(const SolutionOptHS& s) override {
    const RepOptHS& r = s.getR();
    int fo_inv = 0;  // Infeasible Value

    vector<int> lastClean(p.members.size(), -1);
    // cout << "MEMBERS: " << p.members << endl;

    // first MUST be 'Y' and 'M'
    if (r.size() > 3) {
      if ((r[0].first != 'Y') || (r[0].second != 'Z')) fo_inv += 1000;
      if ((r[1].first != 'E') || (r[1].second != 'J')) fo_inv += 1000;
      if ((r[2].first != 'M') || (r[2].second != 'B')) fo_inv += 1000;
    }

    for (unsigned i = 0; i < r.size(); i++) {
      // cout << "ITER " << i << ": " << lastClean << endl;
      for (unsigned m = 0; m < p.members.size(); m++) {
        if ((r[i].first == p.members[m]) || (r[i].second == p.members[m])) {
          if (lastClean[m] > -1) {
            int diff = i - lastClean[m];

            if (diff < 2) {
              fo_inv += diff;
              // cout << "INF for member: " << p.members[m] << " (at week " << i
              // << ")" << endl;
            }
          }

          lastClean[m] = i;
        }
      }
      // cout << "ITER (FINISH)" << i << ": " << lastClean << endl;
    }

    return Evaluation<>(0, fo_inv);
  }

  virtual bool betterThan(double f1, double f2) {
    // MINIMIZATION
    return (f1 < (f2 - OptHS_EPSILON));
  }

  virtual bool isMinimization() const { return true; }

  virtual string id() const override {
    string pai = Evaluator<SolutionOptHS, EvaluationOptHS>::idComponent();
    pai.append(":OptHSEvaluator");
    return pai;
  }
};

}  // namespace OptHS

#endif /*OptHS_EVALUATOR_HPP_*/
