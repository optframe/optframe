// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCH_HPP_
#define OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCH_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C
#include <math.h>
// C++
#include <string>
#include <vector>
//

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Search/ITrajectory.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>
#include <OptFrame/Search/SingleObjSearchBuilder.hpp>
#include <OptFrame/Timer.hpp>

#include "ILS.hpp"

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

MOD_EXPORT template <class H, XESolution XES,
                     XEvaluation XEv = typename XES::second_type>
class IteratedLocalSearch : public ILS,
                            public SingleObjSearch<XES>,
                            public ITrajectory<XES> {
 protected:
  sref<GeneralEvaluator<XES>> evaluator;
  // Constructive<S>& constructive;
  sref<InitialSearch<XES>> constructive;

 public:
  IteratedLocalSearch(sref<GeneralEvaluator<XES>> _evaluator,
                      sref<InitialSearch<XES>> _constructive)
      : evaluator(_evaluator), constructive(_constructive) {}

  ~IteratedLocalSearch() override {}

  virtual uptr<H> initializeHistory() = 0;

  virtual void localSearch(XES& se, const StopCriteria<XEv>& stopCriteria) = 0;

  virtual void perturbation(XES& se, const StopCriteria<XEv>& stopCriteria,
                            H& history) = 0;

  virtual bool acceptanceCriterion(const XEv& e1, const XEv& e2,
                                   H& history) = 0;

  virtual bool terminationCondition(const H& history) = 0;

  // default search method (no initial solution passed)
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XES> opstar) override {
    if (Component::debug) std::cout << "ILS::begin() has debug!" << std::endl;
    if (Component::information)
      std::cout << "ILS::begin() has info!" << std::endl;

    double timeBest = -1;
    if (Component::information)
      std::cout << "ILS::searchBy(" << stopCriteria.timelimit << ")"
                << std::endl;

    if (Component::debug)
      std::cout << "ILS::build initial solution" << std::endl;

    if (!opstar) opstar = constructive->initialSearch(stopCriteria).first;
    if (!opstar) return SearchStatus::NO_SOLUTION;

    XES& star = *opstar;

    XEv& eStar = star.second;
    if (Component::information) {
      std::cout << "ILS::starting with evaluation:" << std::endl;
      eStar.print();
    }

    uptr<H> history = initializeHistory();

    // 's0' <- GenerateSolution
    // 's*' <- localSearch 's'

    if (Component::information)
      std::cout << "ILS::performing first local search" << std::endl;
    localSearch(star, stopCriteria);

    if (Component::information)
      std::cout << "ILS::finished first local search" << std::endl;

    if (Component::information) {
      std::cout << "ILS optimized starts: ";
      eStar.print();
    }

    bool stopByHistory = false;
    bool stopByGeneral = false;
    do {
      // if (Component::information) std::cout << "ILS::begin loop" <<
      // std::endl;
      XES p1 = star;  // derive new incumbent solution (copy-based solution, for
                      // generality)
      perturbation(p1, stopCriteria, *history);
      localSearch(p1, stopCriteria);
      bool improvement = acceptanceCriterion(p1.second, star.second, *history);
      if (improvement) {
        if (Component::information)
          std::cout << "ILS::improvement! star <= new bound at time="
                    << stopCriteria.getTime() << std::endl;
        timeBest = stopCriteria.getTime();
        star = p1;  // copy-based
      }
      if (Component::debug) std::cout << "ILS::SHOULD STOP?" << std::endl;
      stopByHistory = terminationCondition(*history);
      stopByGeneral = stopCriteria.shouldStop(star.second);
      if (Component::information)
        if (stopByHistory || stopByGeneral) {
          std::cout << "info ILS::stop! time=" << stopCriteria.getTime();
          std::cout << " stopByHistory=" << stopByHistory;
          std::cout << " stopByGeneral=" << stopByGeneral;
          std::cout << std::endl;
        }
    } while (!stopByHistory && !stopByGeneral);

    if (!stopCriteria.target_f.isOutdated()) {
      if (Component::debug)
        std::cout << "ILS:: will compare(" << eStar.isOutdated() << ";"
                  << stopCriteria.target_f.isOutdated() << ")" << std::endl;
      if (evaluator->betterStrict(eStar, stopCriteria.target_f)) {
        std::cout << "ILS:: exit by target_f: " << eStar.evaluation()
                  << " better than " << stopCriteria.target_f.evaluation()
                  << std::endl;
        // std::cout << "isMin: " << eStar.isMini << std::endl;
      }
    }

    return {SearchStatus::NO_REPORT, star, timeBest};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":" << ILS::family();
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCH_HPP_
