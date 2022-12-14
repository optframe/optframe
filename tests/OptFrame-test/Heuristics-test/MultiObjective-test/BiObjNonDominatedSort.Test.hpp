// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_HEURISTICS_TEST_MULTIOBJECTIVE_TEST_BIOBJNONDOMINATEDSORT_TEST_HPP_
#define TESTS_OPTFRAME_TEST_HEURISTICS_TEST_MULTIOBJECTIVE_TEST_BIOBJNONDOMINATEDSORT_TEST_HPP_

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
#include <OptFCore/FDirection.hpp>
#include <OptFrame/Heuristics/MultiObjective/BiObjNonDominatedSort.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

using TestXMES = std::pair<int, MultiEvaluation<int>>;
using TestXEv = Evaluation<int>;
using TestXMEv = MultiEvaluation<int>;

TEST_CASE("OptFrameHeuristicsTests: biobjective_sort_front_7_7") {
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

  vsref<Direction<TestXEv>> vDirMinMin;
  vDirMinMin.push_back(new FDirection<TestXEv, MinOrMax::MINIMIZE>{});
  vDirMinMin.push_back(new FDirection<TestXEv, MinOrMax::MINIMIZE>{});
  //
  REQUIRE(vDirMinMin.size() == 2);
  BiObjNonDominatedSort<TestXMES> biObjMinMin{vDirMinMin};
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

  REQUIRE(vDirMinMin[0]->isMinimization());
  REQUIRE(vDirMinMin[1]->isMinimization());
  // NOT WORKING FOR MAX-MAX, AT THE MOMENT!
  //
  /*
  vsref<Direction<TestXEv>> vDirMaxMax;
  vDirMaxMax.push_back(new FDirection<TestXEv, MinOrMax::MAXIMIZE>{});
  vDirMaxMax.push_back(new FDirection<TestXEv, MinOrMax::MAXIMIZE>{});
  //
  REQUIRE(vDirMaxMax.size() == 2);
  BiObjNonDominatedSort<TestXMES> biObjMaxMax{vDirMaxMax};
  biObjMaxMax.assignFitnessAll(vpop);
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
  */
}

#endif  // TESTS_OPTFRAME_TEST_HEURISTICS_TEST_MULTIOBJECTIVE_TEST_BIOBJNONDOMINATEDSORT_TEST_HPP_
