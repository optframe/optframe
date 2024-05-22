// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#include <PN/Evaluator.h>

using namespace PN;

PNEvaluator::PNEvaluator(ProblemInstance& _pPN)
    : Evaluator(new Minimization()), pPN{_pPN} {
  // Put the rest of your code here
}

PNEvaluator::~PNEvaluator() {}

EvaluationPN PNEvaluator::evaluate(const SolutionPN& s) {
  const RepPN& rep = s.getR();
  evaluations++;
  double fo = 0, soma1 = 0, soma2 = 0;

  for (int i = 0; i < (int)rep.size(); i++) {
    if (rep[i] == 1)
      soma1 += pPN.nums[i];
    else
      soma2 += pPN.nums[i];
  }

  fo = abs(soma1 - soma2);

  return EvaluationPN(fo);
}

bool PNEvaluator::betterThan(const EvaluationPN& f1, const EvaluationPN& f2) {
  // MINIMIZATION
  return (f1.evaluation() < (f2.evaluation() - EPSILON_PN));
}
