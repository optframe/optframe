// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#include <iostream>
//
// must come first...
#include <OptFrame/printable/printable.hpp>
//
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/LocalSearch.hpp>
#include <OptFrame/Util/CheckCommand.hpp>
//
#include "TSP-fcore.hpp"  // NOLINT
// implementation of TSP

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
// using namespace TSP_fcore;
int main() {
  std::stringstream ss;
  srand(1000000);
  int N = 50;
  ss << N << std::endl;
  for (unsigned i = 0; i < N; i++)
    ss << i << "\t" << rand() % 1000 << "\t" << rand() % 1000 << std::endl;
  // Scanner scanner{ std::string(instance5) };
  Scanner scanner{ss.str()};
  sref<ProblemContext> pTSP{new ProblemContext{}};
  pTSP->load(scanner);

  // REQUIRE(pTSP.n == 5);
  assert(pTSP->n == 50);

  // set random seed for std::random_shuffle
  srand(1000000);

  FConstructive<std::vector<int>, ProblemContext> crand{pTSP, frandom};
  FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE, ProblemContext> eval{pTSP,
                                                                    fevaluate};
  FNS<ESolutionTSP, ProblemContext> nsswap{pTSP, fRandomSwap};
  sref<FNSSeq<std::pair<int, int>, ESolutionTSP, ProblemContext>> nsseq2{
      make_nsseq(pTSP)};

  sref<InitialSearch<ESolutionTSP>> initRand{
      new BasicInitialSearch<ESolutionTSP>(nnptr::copy(crand),
                                           nnptr::copy(eval))};

  CheckCommand<ESolutionTSP> check(false);  // verbose
  //
  check.addEvaluator(nnptr::copy(eval));
  check.add(initRand);
  check.addNS(nnptr::copy(nsswap));  // NS
  check.addNSSeq(nsseq2);            // NSSeq

  // bool run(int iterMax, int nSolNSSeq)
  check.run(100, 10);
  //
  return 0;
}