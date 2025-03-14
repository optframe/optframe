// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCHLEVELS_HPP_
#define OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCHLEVELS_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <math.h>
//
#include <string>
#include <vector>
//
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Search/SingleObjSearchBuilder.hpp>

#include "./ILSLPerturbation.hpp"
#include "./IteratedLocalSearch.hpp"
#include "ILS.hpp"

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

typedef std::pair<std::pair<int, int>, std::pair<int, int>> levelHistory;

MOD_EXPORT template <XESolution XES,
                     XEvaluation XEv = typename XES::second_type>
class IteratedLocalSearchLevels
    : public IteratedLocalSearch<levelHistory, XES, XEv> {
 protected:
  sref<LocalSearch<XES>> ls;
  sref<ILSLPerturbation<XES, XEv>> p;
  int iterMax, levelMax;

 public:
  IteratedLocalSearchLevels(sref<GeneralEvaluator<XES>> e,
                            sref<InitialSearch<XES>> constructive,
                            sref<LocalSearch<XES>> _ls,
                            sref<ILSLPerturbation<XES, XEv>> _p, int _iterMax,
                            int _levelMax)
      : IteratedLocalSearch<levelHistory, XES, XEv>(e, constructive),
        ls(_ls),
        p(_p),
        iterMax(_iterMax),
        levelMax(_levelMax) {}

  virtual ~IteratedLocalSearchLevels() {}

  uptr<levelHistory> initializeHistory() override {
    // std::cout << "initializeHistory()" << std::endl;
    std::pair<int, int> vars(0, 0);

    // IterMax e LevelMax
    std::pair<int, int> maxs(iterMax, levelMax);

    return uptr<levelHistory>(new levelHistory(vars, maxs));
  }

  void localSearch(XES& se, const StopCriteria<XEv>& stopCriteria) override {
    // std::cout << "localSearch(.)" << std::endl;
    ls->searchFrom(se, stopCriteria);
  }

  void perturbation(XES& se, const StopCriteria<XEv>& stopCriteria,
                    levelHistory& history) override {
    // std::cout << "perturbation(.)" << std::endl;

    int iter = history.first.first;
    int level = history.first.second;
    int iterMax = history.second.first;
    // int levelMax = history.second.second;

    if (Component::debug)
      std::cout << "ILSL::perturbation() history iter " << iter << " level "
                << level << std::endl;

    // std::cout << "level = " << level << " e iter = " << iter << std::endl;

    // nivel atual: 'level'
    p->perturb(se, stopCriteria, level);

    // Incrementa a iteracao
    iter++;

    if (Component::debug)
      std::cout << "ILSL::perturbation() iter " << iter << std::endl;

    if (iter >= iterMax) {
      iter = 0;
      level++;
      if (Component::information)
        std::cout << "ILSL::perturbation() level " << level << ".."
                  << std::endl;
    }

    // Atualiza o historico
    history.first.first = iter;
    history.first.second = level;
    if (Component::debug)
      std::cout << "ILSL::new history iter " << iter << " level " << level
                << std::endl;
  }

  bool acceptanceCriterion(const XEv& e1, const XEv& e2,
                           levelHistory& history) override {
    // std::cout << "acceptanceCriterion(.)" << std::endl;

    // if (IteratedLocalSearch<levelHistory, XES, XEv>::evaluator.betterThan(e1,
    // e2)) if (e1.betterStrict(e2))
    if (Component::debug)
      std::cout << "ILSL::acceptanceCriterion() will compare(outdated="
                << e1.isOutdated() << " e=" << e1.evaluation()
                << "; outdated=" << e2.isOutdated() << " e=" << e2.evaluation()
                << ")" << std::endl;

    if (IteratedLocalSearch<levelHistory, XES, XEv>::evaluator->betterStrict(
            e1, e2)) {
      if (Component::information) {
        std::cout << "ILSL::acceptanceCriterion() Best fo: on [iter "
                  << history.first.first << " of level " << history.first.second
                  << "] => ";
        e1.print();
      }

      // =======================
      //  Atualiza o historico
      // =======================
      // iter = 0
      history.first.first = 0;
      // level = 0
      history.first.second = 0;

      // =======================
      //    Retorna s2
      // =======================
      return true;
    } else {
      return false;
    }
  }

  bool terminationCondition(const levelHistory& history) override {
    // std::cout << "terminationCondition(.)" << std::endl;
    int level = history.first.second;
    int levelMax = history.second.second;

    return (level >= levelMax);
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << IteratedLocalSearch<levelHistory, XES, XEv>::idComponent() << "ILSL";
    return ss.str();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCHLEVELS_HPP_
