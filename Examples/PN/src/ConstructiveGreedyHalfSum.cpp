// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#include <PN/ConstructiveGreedyHalfSum.h>

using namespace PN;

ConstructiveGreedyHalfSum::ConstructiveGreedyHalfSum(ProblemInstance& _pPN)
    : pPN(_pPN) {}

ConstructiveGreedyHalfSum::~ConstructiveGreedyHalfSum() {}

// Solution<ESolutionPN>* ConstructiveGreedyHalfSum::generateSolution(double
// timelimit)
std::optional<SolutionPN> ConstructiveGreedyHalfSum::generateSolution(
    double timelimit) {
  RepPN sol = vector<bool>(this->pPN.nums.size());

  double soma = 0, soma1 = 0, soma2 = 0;

  random_shuffle(pPN.nums.begin(), pPN.nums.end());

  for (int i = 0; i < (int)pPN.nums.size(); ++i) {
    soma += pPN.nums[i];
  }

  soma /= 2;

  int i = 0;

  while ((soma1 + pPN.nums[i]) <= soma) {
    sol[i] = 1;
    soma1 += pPN.nums[i];
    i++;
  }

  for (int j = i; j < (int)pPN.nums.size(); ++j) {
    if ((soma1 + pPN.nums[j]) <= soma) {
      sol[j] = 1;
      soma1 += pPN.nums[j];
    } else {
      sol[j] = 0;
      soma2 += pPN.nums[j];
    }
  }

  SolutionPN s{sol};
  return std::make_optional(s);
  // return new Solution<ESolutionPN>(sol);
}
