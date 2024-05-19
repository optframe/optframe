// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_HEURISTICS_TEST_MULTIOBJECTIVE_TEST_NONDOMINATEDSORT_TEST_HPP_
#define TESTS_OPTFRAME_TEST_HEURISTICS_TEST_MULTIOBJECTIVE_TEST_NONDOMINATEDSORT_TEST_HPP_

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
#include <OptFCore/FDirection.hpp>
#include <OptFCore/FEvaluator.hpp>
#include <OptFrame/Helper/MultiEvaluation.hpp>
#include <OptFrame/Heuristics/MultiObjective/NonDominatedSort.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

using TestS = int;
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
static_assert(XEvaluation<MultiEvaluation<int>>);
#endif
using TestXMES = std::pair<TestS, MultiEvaluation<int>>;
using TestXEv = Evaluation<int>;
using TestXMEv = MultiEvaluation<int>;
using TestXES = std::pair<TestS, TestXEv>;

TEST_CASE("OptFrameHeuristicsTests: nondom_sort_front_7_7") {
  // example in (7x7) grid
  MultiEvaluation<int> A{vector<int>{1, 6}};
  MultiEvaluation<int> B{vector<int>{2, 4}};
  MultiEvaluation<int> C{vector<int>{3, 2}};
  MultiEvaluation<int> D{vector<int>{5, 1}};
  MultiEvaluation<int> E{vector<int>{2, 7}};
  MultiEvaluation<int> F{vector<int>{3, 5}};
  MultiEvaluation<int> G{vector<int>{4, 3}};
  MultiEvaluation<int> H{vector<int>{5, 6}};
  MultiEvaluation<int> I{vector<int>{5, 4}};
  MultiEvaluation<int> J{vector<int>{7, 5}};

  vector<MOSIndividual<TestXMES>> vpop;
  vpop.push_back(MOSIndividual{TestXMES{-1, A}});
  vpop.push_back(MOSIndividual{TestXMES{-1, B}});
  vpop.push_back(MOSIndividual{TestXMES{-1, C}});
  vpop.push_back(MOSIndividual{TestXMES{-1, D}});
  vpop.push_back(MOSIndividual{TestXMES{-1, E}});
  vpop.push_back(MOSIndividual{TestXMES{-1, F}});
  vpop.push_back(MOSIndividual{TestXMES{-1, G}});
  vpop.push_back(MOSIndividual{TestXMES{-1, H}});
  vpop.push_back(MOSIndividual{TestXMES{-1, I}});
  vpop.push_back(MOSIndividual{TestXMES{-1, J}});

  // ======================================================
  //                      MIN-MIN

  vsref<Evaluator<TestS, TestXEv, TestXES>> veval2min;
  veval2min.push_back(new FEvaluator<TestXES, MinOrMax::MINIMIZE>{
      [](const TestS& s) -> TestXEv {
        int obj = -1;
        return TestXEv{obj};
      }});
  veval2min.push_back(new FEvaluator<TestXES, MinOrMax::MINIMIZE>{
      [](const TestS& s) -> TestXEv {
        int obj = -2;
        return TestXEv{obj};
      }});
  sref<MultiEvaluator<TestXES, TestXMES>> mev2min{
      new MultiEvaluator<TestXES, TestXMES>{veval2min}};
  //
  REQUIRE(mev2min->vDir.size() == 2);
  NonDominatedSort<TestXMES> biObjMinMin{mev2min};
  biObjMinMin.assignFitnessAll(vpop);
  REQUIRE(vpop[0].fitness == 0);  // A
  REQUIRE(vpop[1].fitness == 0);  // B
  REQUIRE(vpop[2].fitness == 0);  // C
  REQUIRE(vpop[3].fitness == 0);  // D
  REQUIRE(vpop[4].fitness == 1);  // E
  REQUIRE(vpop[5].fitness == 1);  // F
  REQUIRE(vpop[6].fitness == 1);  // G
  REQUIRE(vpop[7].fitness == 3);  // H
  REQUIRE(vpop[8].fitness == 2);  // I
  REQUIRE(vpop[9].fitness == 3);  // J

  // ======================================================
  //                      MAX-MAX

  vsref<Evaluator<TestS, TestXEv, TestXES>> veval2max;
  veval2max.push_back(new FEvaluator<TestXES, MinOrMax::MAXIMIZE>{
      [](const TestS& s) -> TestXEv {
        int obj = -1;
        return TestXEv{obj};
      }});
  veval2max.push_back(new FEvaluator<TestXES, MinOrMax::MAXIMIZE>{
      [](const TestS& s) -> TestXEv {
        int obj = -2;
        return TestXEv{obj};
      }});
  sref<MultiEvaluator<TestXES, TestXMES>> mev2max{
      new MultiEvaluator<TestXES, TestXMES>{veval2max}};
  //
  REQUIRE(mev2max->vDir.size() == 2);
  NonDominatedSort<TestXMES> biObjMaxMax{mev2max};
  //
  biObjMinMin.assignFitnessAll(vpop);
  REQUIRE(vpop[0].fitness == 0);  // A
  REQUIRE(vpop[1].fitness == 0);  // B
  REQUIRE(vpop[2].fitness == 0);  // C
  REQUIRE(vpop[3].fitness == 0);  // D
  REQUIRE(vpop[4].fitness == 1);  // E
  REQUIRE(vpop[5].fitness == 1);  // F
  REQUIRE(vpop[6].fitness == 1);  // G
  REQUIRE(vpop[7].fitness == 3);  // H
  REQUIRE(vpop[8].fitness == 2);  // I
  REQUIRE(vpop[9].fitness == 3);  // J
}

TEST_CASE("OptFrameHeuristicsTests: nondom_sort_front_min_min_min") {
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
  NonDominatedSort<TestXMES> ndSort{mev};
  //
  REQUIRE(ndSort.vDir.size() == 3);

  //
  MultiEvaluation<int> A{vector<int>{1, 2, 3}};
  MultiEvaluation<int> B{vector<int>{2, 1, 3}};
  MultiEvaluation<int> C{vector<int>{3, 3, 3}};
  MultiEvaluation<int> D{vector<int>{4, 5, 6}};
  MultiEvaluation<int> E{vector<int>{6, 5, 4}};

  vector<MOSIndividual<TestXMES>> vpop;
  vpop.push_back(MOSIndividual{TestXMES{-1, A}});
  vpop.push_back(MOSIndividual{TestXMES{-1, B}});
  vpop.push_back(MOSIndividual{TestXMES{-1, C}});
  vpop.push_back(MOSIndividual{TestXMES{-1, D}});
  vpop.push_back(MOSIndividual{TestXMES{-1, E}});

  ndSort.assignFitnessAll(vpop);
  REQUIRE(vpop[0].fitness == 0);  // A
  REQUIRE(vpop[1].fitness == 0);  // B
  REQUIRE(vpop[2].fitness == 1);  // C
  REQUIRE(vpop[3].fitness == 2);  // D
  REQUIRE(vpop[4].fitness == 2);  // E
}

#endif  // TESTS_OPTFRAME_TEST_HEURISTICS_TEST_MULTIOBJECTIVE_TEST_NONDOMINATEDSORT_TEST_HPP_
