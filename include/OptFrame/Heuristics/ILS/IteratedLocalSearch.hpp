// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_ILS_HPP_
#define OPTFRAME_ILS_HPP_

#include <math.h>

#include <vector>

#include "../../Constructive.hpp"
#include "../../Evaluator.hpp"
#include "../../ITrajectory.hpp"
#include "../../SingleObjSearch.hpp"
#include "../../Timer.hpp"
#include "ILS.h"

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

  virtual ~IteratedLocalSearch() {}

  virtual sref<H> initializeHistory() = 0;

  virtual void localSearch(XES& se, const StopCriteria<XEv>& stopCriteria) = 0;

  virtual void perturbation(XES& se, const StopCriteria<XEv>& stopCriteria,
                            sref<H> history) = 0;

  virtual bool acceptanceCriterion(const XEv& e1, const XEv& e2,
                                   sref<H> history) = 0;

  virtual bool terminationCondition(sref<H> history) = 0;

  // default search method (no initial solution passed)
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XES> _best) override {
    if (Component::information)
      std::cout << "ILS opt search(" << stopCriteria.timelimit << ")"
                << std::endl;
    //
    op<XES> star;  // TODO: receive on 'searchBy'

    if (Component::debug)
      std::cout << "ILS::build initial solution" << std::endl;

    // star = star ?: constructive->initialSearch(stopCriteria).first;
    if (!star) star = constructive->initialSearch(stopCriteria).first;
    if (!star) return SearchStatus::NO_SOLUTION;

    XEv& eStar = star->second;
    if (Component::information) {
      std::cout << "ILS::starting with evaluation:" << std::endl;
      eStar.print();
    }

    return searchByIncumbent(*star, *star, stopCriteria);
  }

  // for ILS: incumbent is always derived from star, ignoring 'incumbent'
  SearchOutput<XES> searchByIncumbent(
      XES& star, XES&, const StopCriteria<XEv>& stopCriteria) override {
    if (Component::information)
      std::cout << "ILS opt searchBy(" << stopCriteria.timelimit << ")"
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
      XES p1 = star;  // derive new incumbent solution (copy-based solution, for
                      // generality)
      perturbation(p1, stopCriteria, *history);
      localSearch(p1, stopCriteria);
      bool improvement = acceptanceCriterion(p1.second, star.second, history);
      if (improvement) star = p1;  // copy-based
      if (Component::debug) std::cout << "SHOULD STOP?" << std::endl;
    } while (!terminationCondition(history) &&
             !stopCriteria.shouldStop(star.second));

    if (!stopCriteria.target_f.isOutdated()) {
      if (Component::debug)
        std::cout << "ILS will compare(" << eStar.isOutdated() << ";"
                  << stopCriteria.target_f.isOutdated() << ")" << std::endl;
      if (evaluator->betterStrict(eStar, stopCriteria.target_f)) {
        cout << "ILS exit by target_f: " << eStar.evaluation()
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

#endif /*OPTFRAME_ILS_HPP_*/
