// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#include <PN/ConstructiveRandom.h>

using namespace PN;

ConstructiveRandom::ConstructiveRandom(ProblemInstance& _pPN) : pPN(_pPN) {}

ConstructiveRandom::~ConstructiveRandom() {}

// Solution<ESolutionPN>* ConstructiveRandom::generateSolution(double timelimit)
std::optional<SolutionPN> ConstructiveRandom::generateSolution(
    double timelimit) {
  RepPN sol = vector<bool>(pPN.nums.size());

  for (int i = 0; i < (int)pPN.nums.size(); i++) {
    double u = (double)(rand() % 100) / 100.0;

    if (u <= 0.5)
      sol[i] = 0;
    else
      sol[i] = 1;
  }

  SolutionPN s{sol};
  return std::make_optional(s);
  // return new Solution<ESolutionPN>(sol);
}
