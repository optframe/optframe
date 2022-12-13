// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrme developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_HEURISTICS_TEST_TABUSEARCH_TEST_HPP_  // NOLINT
#define TESTS_OPTFRAME_TEST_HEURISTICS_TEST_TABUSEARCH_TEST_HPP_  // NOLINT

// C++
#include <limits>
#include <utility>
#include <vector>

// testing
#include <catch2/catch_amalgamated.hpp>

// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md
// Now v3: ...

// core includes
#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
#include <OptFrame/Heuristics/TS/BasicTabuSearch.hpp>

#include "include/OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp"

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

TEST_CASE("OptFrameHeuristicsTests: tabu_search_kp") {
  Scanner sProb{"5 \n 10 \n 1 2 3 4 5 \n 5 4 3 2 1"};
  sref<KP_fcore::ProblemContext> pKP{new KP_fcore::ProblemContext{}};
  pKP->load(sProb);
  REQUIRE(pKP->n == 5);
  //
  KP_fcore::OptFrameCoreKP core{pKP};

  sref<GeneralEvaluator<KP_fcore::ESolutionKP>> _ev = core.evalKP;
  sref<InitialSearch<KP_fcore::ESolutionKP>> is{
      new BasicInitialSearch<KP_fcore::ESolutionKP>{core.evalKP,
                                                    core.randomConstructive}};

  BasicTabuSearch<KP_fcore::ESolutionKP> basicTabuSearch(_ev, is,
                                                         core.nsseqFlip, 2, 10);

  REQUIRE(true);
}

#endif  // TESTS_OPTFRAME_TEST_HEURISTICS_TEST_TABUSEARCH_TEST_HPP_ // NOLINT
