// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#include <PN/ConstructiveGreedySum.h>

using namespace PN;

ConstructiveGreedySum::ConstructiveGreedySum(ProblemInstance& _pPN)
    : pPN(_pPN) {}

ConstructiveGreedySum::~ConstructiveGreedySum() {}

// Solution<ESolutionPN>* ConstructiveGreedySum::generateSolution(double
// timelimit)
std::optional<SolutionPN> ConstructiveGreedySum::generateSolution(
    double timelimit) {
  RepPN sol = vector<bool>(pPN.nums.size());

  double soma1 = 0, soma2 = 0;

  sort(pPN.nums.rbegin(), pPN.nums.rend());

  for (int i = 0; i < (int)pPN.nums.size(); i++) {
    if (soma1 > soma2) {
      sol[i] = 0;
      soma2 += pPN.nums[i];
    } else {
      sol[i] = 1;
      soma1 += pPN.nums[i];
    }
  }

  SolutionPN s{sol};
  return std::make_optional(s);
  // return new Solution<ESolutionPN>(sol);
}
