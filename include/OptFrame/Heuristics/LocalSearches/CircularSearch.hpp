// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSEnum.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

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

MOD_EXPORT template <XESolution XES>
class CircularSearch : public LocalSearch<XES> {
  using XSH = XES;  // primary-based search type only (BestType)
  using XEv = typename XES::second_type;

 private:
  sref<GeneralEvaluator<XES>> eval;
  sref<NSEnum<XES, XSH>> ns;

  int initial_w;

 public:
  CircularSearch(sref<GeneralEvaluator<XES>> _eval,
                 sref<NSEnum<XES, XSH>> _nsEnum)
      : eval(_eval), ns(_nsEnum) {
    initial_w = 0;
  }

  virtual ~CircularSearch() {}

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  virtual SearchStatus searchFrom(XES& se,
                                  const StopCriteria<XEv>& sosc) override {
    // XSolution& s = se.first;
    // XEv& e = se.second;

    // double timelimit = sosc.timelimit;
    // double target_f = sosc.target_f;
    int Wmax = ns->size();

    int w = initial_w % Wmax;

    do {
      uptr<Move<XES, XSH>> m = ns->indexMove(w);

      // if (m->canBeApplied(s)) {
      if (m->canBeApplied(se)) {
        bool mayEstimate = false;
        /// MoveCost<>& cost = *eval->moveCost(m, se, mayEstimate);
        op<XEv> cost = eval->moveCost(*m, se, mayEstimate);

        // if (eval->isImprovement(*cost)) {
        // if (cost->isImprovingStrict()) {
        if (eval->isStrictImprovement(*cost)) {
          // double old_f = e.evaluation();

          m->applyUpdate(se);
          eval->reevaluate(se);  // updates 'e'

          // std::cout << "CS improvement! w:" << w << " fo=" << e.evaluation()
          // << " (antiga fo="<< old_f << ")" << std::endl << std::endl;

          initial_w = w + 1;

          return SearchStatus::IMPROVEMENT;
        }
      }

      w = (w + 1) % Wmax;
    } while (w != initial_w);
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":CS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_CIRCULARSEARCH_HPP_
