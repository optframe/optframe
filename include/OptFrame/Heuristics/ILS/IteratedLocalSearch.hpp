// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCH_HPP_
#define OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCH_HPP_

// C
#include <math.h>
// C++
#include <string>
#include <vector>
//

#include "../../Constructive.hpp"
#include "../../Evaluator.hpp"
#include "../../ITrajectory.hpp"
#include "../../SingleObjSearch.hpp"
#include "../../Timer.hpp"
#include "./ILS.h"

namespace optframe {

template <class H, XESolution XES, XEvaluation XEv = typename XES::second_type>
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

  virtual sref<H> initializeHistory() = 0;

  virtual void localSearch(XES& se, const StopCriteria<XEv>& stopCriteria) = 0;

  virtual void perturbation(XES& se, const StopCriteria<XEv>& stopCriteria,
                            sref<H> history) = 0;

  virtual bool acceptanceCriterion(const XEv& e1, const XEv& e2,
                                   sref<H> history) = 0;

  virtual bool terminationCondition(sref<H> history) = 0;

  // default search method (no initial solution passed)
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XES> opstar) override {
    if (Component::information)
      std::cout << "ILS::searchBy(" << stopCriteria.timelimit << ")"
                << std::endl;

    if (Component::debug)
      std::cout << "ILS::build initial solution" << std::endl;

    if (!opstar) opstar = constructive->initialSearch(stopCriteria).first;
    if (!opstar) return SearchStatus::NO_SOLUTION;

    XES& star = *opstar;
    if (Component::information)
      std::cout << "ILS::searchBy(" << stopCriteria.timelimit << ")"
                << std::endl;

    XEv& eStar = star.second;
    if (Component::information) {
      std::cout << "ILS::starting with evaluation:" << std::endl;
      eStar.print();
    }

    sref<H> history = initializeHistory();

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

    do {
      if (Component::debug) std::cout << "ILS::begin loop" << std::endl;
      XES p1 = star;  // derive new incumbent solution (copy-based solution, for
                      // generality)
      perturbation(p1, stopCriteria, history);
      localSearch(p1, stopCriteria);
      bool improvement = acceptanceCriterion(p1.second, star.second, history);
      if (improvement) {
        if (Component::debug)
          std::cout << "ILS::improvement! star <= new bound" << std::endl;
        star = p1;  // copy-based
      }
      if (Component::debug) std::cout << "ILS::SHOULD STOP?" << std::endl;
    } while (!terminationCondition(history) &&
             !stopCriteria.shouldStop(star.second));

    if (!stopCriteria.target_f.isOutdated()) {
      if (Component::debug)
        std::cout << "ILS:: will compare(" << eStar.isOutdated() << ";"
                  << stopCriteria.target_f.isOutdated() << ")" << std::endl;
      if (evaluator->betterStrict(eStar, stopCriteria.target_f)) {
        cout << "ILS:: exit by target_f: " << eStar.evaluation()
             << " better than " << stopCriteria.target_f.evaluation() << endl;
        // cout << "isMin: " << eStar.isMini << endl;
      }
    }

    return {SearchStatus::NO_REPORT, star};
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":" << ILS::family();
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCH_HPP_
