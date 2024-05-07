// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_PARETODOMINANCE_TEST_HPP_
#define TESTS_OPTFRAME_TEST_PARETODOMINANCE_TEST_HPP_

// C++
#include <limits>
#include <utility>
#include <vector>

// testing
// #include <catch2/catch_amalgamated.hpp>
#ifdef MAKE
#include <catch2/catch_amalgamated.hpp>
#else
#include <catch2/catch_test_macros.hpp>
#endif

// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md
// Now v3: ...

// core includes
#include <OptFCore/FEvaluator.hpp>
#include <OptFrame/ParetoDominance.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

using TestS = int;
using TestXMES = std::pair<TestS, MultiEvaluation<int>>;
using TestXEv = Evaluation<int>;
using TestXMEv = MultiEvaluation<int>;
using TestXES = std::pair<TestS, TestXEv>;

TEST_CASE("OptFrameTests: pareto_dominance") {
  //
  vsref<Evaluator<TestS, TestXEv, TestXES>> veval;
  veval.push_back(new FEvaluator<TestXES, MinOrMax::MINIMIZE>{
      [](const TestS& s) -> TestXEv {
        int obj = -1;
        return TestXEv{obj};
      }});
  veval.push_back(new FEvaluator<TestXES, MinOrMax::MINIMIZE>{
      [](const TestS& s) -> TestXEv {
        int obj = -2;
        return TestXEv{obj};
      }});
  veval.push_back(new FEvaluator<TestXES, MinOrMax::MINIMIZE>{
      [](const TestS& s) -> TestXEv {
        int obj = -3;
        return TestXEv{obj};
      }});

  sref<MultiEvaluator<TestXES, TestXMES>> mev{
      new MultiEvaluator<TestXES, TestXMES>{veval}};
  //
  REQUIRE(mev->vDir.size() == 3);
  //
  ParetoDominance<TestXES, TestXMES> pDom{mev};
  //
  //
  MultiEvaluation<int> A{vector<int>{1, 2, 3}};
  MultiEvaluation<int> B{vector<int>{2, 1, 3}};
  MultiEvaluation<int> C{vector<int>{3, 3, 3}};
  // A and B dominates C; A and B are both non-dominated
  REQUIRE(pDom.birelation(A, B) == std::make_pair(false, false));
  REQUIRE(pDom.birelation(A, C) == std::make_pair(true, false));
  REQUIRE(pDom.birelation(C, B) == std::make_pair(false, true));
  // identity checks (all false)
  REQUIRE(pDom.birelation(A, A) == std::make_pair(false, false));
  REQUIRE(pDom.birelation(B, B) == std::make_pair(false, false));
  REQUIRE(pDom.birelation(C, C) == std::make_pair(false, false));
}

#endif  // TESTS_OPTFRAME_TEST_PARETODOMINANCE_TEST_HPP_
