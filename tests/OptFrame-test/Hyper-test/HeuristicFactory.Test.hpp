// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_HYPER_TEST_HEURISTICFACTORY_TEST_HPP_
#define TESTS_OPTFRAME_TEST_HYPER_TEST_HEURISTICFACTORY_TEST_HPP_

// C++
#include <limits>
#include <string>
#include <utility>
#include <vector>

// testing
#include <catch2/catch_amalgamated.hpp>

// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md
// Now v3: ...

// core includes
#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
#include <OptFrame-Core-Examples/FCore-bTSP/BTSP-fcore.hpp>
// #include <OptFrame/Concepts/BaseConcepts.ctest.hpp>
#include <OptFrame/Hyper/HeuristicFactory.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

TEST_CASE("OptFrameHeuristicFactoryTests: addComponentList") {
  using namespace BTSP_fcore;
  // N=5
  // 1 1 1
  // 2 2 2
  // 3 3 3
  // 4 4 4
  // 5 5 5
  // 1 10 50
  // 2 20 40
  // 3 30 30
  // 4 40 20
  // 5 50 10
  Scanner sProb{
      "5 \n 1 1 1 \n 2 2 2 \n 3 3 3 \n 4 4 4 \n 5 5 5 \n 1 10 50 \n 2 20 40 \n "
      "3 30 30 \n 4 40 20 \n 5 50 10"};
  sref<BTSP_fcore::ProblemContextBTSP> p{new BTSP_fcore::ProblemContextBTSP{}};
  p->load(sProb);
  REQUIRE(p->n == 5);
  //
  BTSP_fcore::OptFrameCoreBTSP core{p};

  HeuristicFactory<ESolutionBTSPSingle> hf;
  std::stringstream ss;
  hf.logdata = &ss;

  // normal print
  hf.setLogLevel(LogLevel::Info);

  // case 1: empty list (should add normally!)
  vector<sptr<Component>> cListEmpty;
  int list_id = hf.addComponentList(cListEmpty, "OptFrame:NS");

  REQUIRE(list_id == 0);
  REQUIRE(ss.str() ==
          "INFO(HeuristicFactory): adding to list type 'OptFrame:NS[]' id 0 "
          "size 0\n");
}

#endif  // TESTS_OPTFRAME_TEST_HYPER_TEST_HEURISTICFACTORY_TEST_HPP_
