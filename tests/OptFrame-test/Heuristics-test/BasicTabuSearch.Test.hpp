// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef TESTS_OPTFRAME_TEST_HEURISTICS_TEST_TABUSEARCH_TEST_HPP_  // NOLINT
#define TESTS_OPTFRAME_TEST_HEURISTICS_TEST_TABUSEARCH_TEST_HPP_  // NOLINT

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
#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
// #include <OptFrame/Concepts/BaseConcepts.ctest.hpp>
#include <OptFrame/Heuristics/TS/BasicTabuSearch.hpp>
#include <OptFrame/InitialSearch.hpp>

using namespace std;       // NOLINT
using namespace optframe;  // NOLINT

TEST_CASE("OptFrameHeuristicsTests: tabu_search_kp") {
  // N=5 Q=23
  // k: 0 1 2 3 4 (move bit flip)
  // p: 2 2 3 4 4
  // w: 4 5 7 9 6
  // INF_W = sum_p = 15
  //
  // Example from Prof. Marcone Jamilson Freitas Souza
  // http://www.decom.ufop.br/marcone/Disciplinas/InteligenciaComputacional/InteligenciaComputacional.pdf
  //
  Scanner sProb{"5 \n 23 \n 2 2 3 4 4 \n 4 5 7 9 6 \n 15"};
  sref<KP_fcore::ProblemContext> p{new KP_fcore::ProblemContext{}};
  p->load(sProb);
  REQUIRE(p->n == 5);
  //
  KP_fcore::OptFrameCoreKP core{p};

  sref<GeneralEvaluator<KP_fcore::ESolutionKP>> ev = nnptr::copy(core.evalKP);
  sref<IEvaluator<KP_fcore::ESolutionKP>> iev = nnptr::copy(core.evalKP);
  sref<InitialSearch<KP_fcore::ESolutionKP>> constructive{
      new BasicInitialSearch<KP_fcore::ESolutionKP>(
          nnptr::copy(core.randomConstructive), nnptr::copy(core.evalKP))};

  BasicTabuSearch<KP_fcore::ESolutionKP> basicTabuSearch(
      ev, constructive, nnptr::copy(core.nsseqFlip), 2, 1);

  // =============
  // base solution
  // =============

  auto evaluator = basicTabuSearch.evaluator;
  //
  KP_fcore::ESolutionKP se_base{std::vector<bool>{0, 1, 0, 1, 0},
                                Evaluation<int>{0}};
  se_base.second.invalidate();
  evaluator->reevaluate(se_base);
  //
  REQUIRE(se_base.second.evaluation() == 6);

  // ==================================================
  // testing best non-tabu move and aspiration criteria
  // ==================================================

  auto se = se_base;
  REQUIRE(se_base.second.evaluation() == 6);
  //
  std::vector<Move<KP_fcore::ESolutionKP>*> tabuList;
  //
  auto estar = se;
  // aspiration by best
  std::function<bool(const Evaluation<int>&, const Evaluation<int>&)> cmpA =
      [&estar, &evaluator](const Evaluation<int>& e_prime,
                           const Evaluation<int>&) {
        return evaluator->betterStrict(e_prime, estar.second);
      };

  // ==========
  // iter 1
  // ==========

  uptr<Move<KP_fcore::ESolutionKP>> m1 =
      basicTabuSearch.tabuBestMoveWithAspiration(se, tabuList, cmpA);

  // will choose k=4 (p=10 w=20)
  uptr<Move<KP_fcore::ESolutionKP>> m_best_k_4{new KP_fcore::MoveBitFlip{4}};
  REQUIRE(*m1 == *m_best_k_4);
  // apply move
  auto m1_rev = m1->applyUpdate(se);
  evaluator->reevaluate(se);
  REQUIRE(se.second.evaluation() == 10);
  // update tabu list (reverse is also k = 4)
  REQUIRE(*m1_rev == *m_best_k_4);
  tabuList.push_back(m1_rev.release());
  // update best solution
  estar = se;

  // ==========
  // iter 2
  // ==========

  uptr<Move<KP_fcore::ESolutionKP>> m2 =
      basicTabuSearch.tabuBestMoveWithAspiration(se, tabuList, cmpA);

  // will choose k=2 (p=8 w=15)
  uptr<Move<KP_fcore::ESolutionKP>> m_best_k_1{new KP_fcore::MoveBitFlip{1}};
  REQUIRE(*m2 == *m_best_k_1);
  // apply move
  auto m2_rev = m2->applyUpdate(se);
  evaluator->reevaluate(se);
  REQUIRE(se.second.evaluation() == 8);
  // update tabu list (reverse is also k = 1)
  REQUIRE(*m2_rev == *m_best_k_1);
  tabuList.push_back(m2_rev.release());
  {
    uptr<Move<KP_fcore::ESolutionKP>> firstTabu{tabuList[0]};
    tabuList.erase(tabuList.begin());
  }
  REQUIRE(tabuList.size() == 1);
  // do not update best solution
  REQUIRE(estar.second.evaluation() == 10);

  // ==========
  // iter 3
  // ==========

  uptr<Move<KP_fcore::ESolutionKP>> m3 =
      basicTabuSearch.tabuBestMoveWithAspiration(se, tabuList, cmpA);

  // will choose k=3 (p=11 w=22)
  uptr<Move<KP_fcore::ESolutionKP>> m_best_k_2{new KP_fcore::MoveBitFlip{2}};
  REQUIRE(*m3 == *m_best_k_2);
  // apply move
  auto m3_rev = m3->applyUpdate(se);
  evaluator->reevaluate(se);
  REQUIRE(se.second.evaluation() == 11);
  // update tabu list (reverse is also k = 2)
  REQUIRE(*m3_rev == *m_best_k_2);
  tabuList.push_back(m3_rev.release());
  {
    uptr<Move<KP_fcore::ESolutionKP>> firstTabu{tabuList[0]};
    tabuList.erase(tabuList.begin());
  }
  REQUIRE(tabuList.size() == 1);
  // update best solution
  estar = se;
  REQUIRE(estar.second.evaluation() == 11);

  // ==========
  // iter 4
  // ==========

  uptr<Move<KP_fcore::ESolutionKP>> m4 =
      basicTabuSearch.tabuBestMoveWithAspiration(se, tabuList, cmpA);

  // best should be f(s')=8, with k=2... but it's tabu
  // so, best available will be f(s')=7, with k=3
  // will choose k=3 (p=7 w=13)
  uptr<Move<KP_fcore::ESolutionKP>> m_best_k_3{new KP_fcore::MoveBitFlip{3}};
  REQUIRE(*m4 == *m_best_k_3);
  // apply move
  auto m4_rev = m4->applyUpdate(se);
  evaluator->reevaluate(se);
  REQUIRE(se.second.evaluation() == 7);
  // update tabu list (reverse is also k = 3)
  REQUIRE(*m4_rev == *m_best_k_3);
  tabuList.push_back(m4_rev.release());
  {
    uptr<Move<KP_fcore::ESolutionKP>> firstTabu{tabuList[0]};
    tabuList.erase(tabuList.begin());
  }
  REQUIRE(tabuList.size() == 1);
  // do not update best solution
  REQUIRE(estar.second.evaluation() == 11);

  // ==========
  // iter 5
  // ==========

  uptr<Move<KP_fcore::ESolutionKP>> m5 =
      basicTabuSearch.tabuBestMoveWithAspiration(se, tabuList, cmpA);

  // will choose k=0 (p=9 w=17)
  uptr<Move<KP_fcore::ESolutionKP>> m_best_k_0{new KP_fcore::MoveBitFlip{0}};
  REQUIRE(*m5 == *m_best_k_0);
  // apply move
  auto m5_rev = m5->applyUpdate(se);
  evaluator->reevaluate(se);
  REQUIRE(se.second.evaluation() == 9);
  // update tabu list (reverse is also k = 0)
  REQUIRE(*m5_rev == *m_best_k_0);
  tabuList.push_back(m5_rev.release());
  {
    uptr<Move<KP_fcore::ESolutionKP>> firstTabu{tabuList[0]};
    tabuList.erase(tabuList.begin());
  }
  REQUIRE(tabuList.size() == 1);
  // do not update best solution
  REQUIRE(estar.second.evaluation() == 11);

  // ==================================
  // testing execution with tabu search
  // ==================================

  auto se_ts = se_base;
  REQUIRE(se_base.second.evaluation() == 6);

  // too many seconds
  auto sout = basicTabuSearch.searchBy({10000.0}, std::move(se_ts));

  REQUIRE(sout.best->second.evaluation() == 11);
}

#endif  // TESTS_OPTFRAME_TEST_HEURISTICS_TEST_TABUSEARCH_TEST_HPP_ // NOLINT
