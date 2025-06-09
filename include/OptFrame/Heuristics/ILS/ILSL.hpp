// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_ILSL_HPP_
#define OPTFRAME_HEURISTICS_ILS_ILSL_HPP_

// This is Iterated Local Search Levels (ILSL)
// an implementation of ILS, using levels of perturbation

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <math.h>
//
#include <string>
#include <vector>
//
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Search/SingleObjSearchBuilder.hpp>

#include "./ILSLPerturbation.hpp"
#include "FamilyILS.hpp"
#include "MetaILS.hpp"

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

typedef std::pair<std::pair<int, int>, std::pair<int, int>> levelHistory;

MOD_EXPORT template <XESolution XES,
                     XEvaluation XEv = typename XES::second_type>
class ILSL : public MetaILS<levelHistory, XES> {
 protected:
  sref<LocalSearch<XES>> ls;
  sref<ILSLPerturbation<XES, XEv>> p;
  int iterMax, levelMax;

 public:
  ILSL(sref<GeneralEvaluator<XES>> e, sref<InitialSearch<XES>> constructive,
       sref<LocalSearch<XES>> _ls, sref<ILSLPerturbation<XES, XEv>> _p,
       int _iterMax, int _levelMax)
      : MetaILS<levelHistory, XES>(e, constructive),
        ls(_ls),
        p(_p),
        iterMax(_iterMax),
        levelMax(_levelMax) {}

  virtual ~ILSL() {}

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

    if ((Component::information) && (iter == iterMax - 1))
      std::cout << "info ILSL::perturbation() last iteration iter= " << iter
                << std::endl;

    // Incrementa a iteracao
    iter++;
    // if ((Component::information))
    //  std::cout << "ILSL::perturbation() new iter= " << iter << std::endl;

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

    // if (MetaILS<levelHistory, XES>::evaluator.betterThan(e1,
    // e2)) if (e1.betterStrict(e2))
    if (Component::debug)
      std::cout << "debug ILSL::acceptanceCriterion() will compare(outdated="
                << e1.isOutdated() << " e=" << e1.evaluation()
                << "; outdated=" << e2.isOutdated() << " e=" << e2.evaluation()
                << ")" << std::endl;

    if (MetaILS<levelHistory, XES>::evaluator->betterStrict(e1, e2)) {
      if (Component::information) {
        std::cout << "info ILSL::acceptanceCriterion() ";
        std::cout << "Best fo: on [iter " << history.first.first << " of level "
                  << history.first.second << "] => ";
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
    if ((Component::information) && (level >= levelMax)) {
      std::cout << "ILSL::terminationCondition() ";
      std::cout << "level=" << level << " levelMax=" << levelMax << std::endl;
    }

    return (level >= levelMax);
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << MetaILS<levelHistory, XES>::idComponent() << "ILSL";
    return ss.str();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_ILSL_HPP_
