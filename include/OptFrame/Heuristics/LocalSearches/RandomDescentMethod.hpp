// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_RANDOMDESCENTMETHOD_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_RANDOMDESCENTMETHOD_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Timer.hpp>

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

MOD_EXPORT template <XESolution XES>
class RandomDescentMethod : public LocalSearch<XES> {
 private:
  using XEv = typename XES::second_type;
  using XSH = XES;  // primary-based search type only (BestType)
  sref<GeneralEvaluator<XES>> evaluator;
  sref<NS<XES>> ns;
  unsigned int iterMax;

 public:
  RandomDescentMethod(sref<GeneralEvaluator<XES>> _eval, sref<NS<XES, XSH>> _ns,
                      unsigned int _iterMax)
      : evaluator(_eval), ns(_ns), iterMax(_iterMax) {}

  virtual ~RandomDescentMethod() = default;

  SearchStatus searchFrom(XES& se,
                          const StopCriteria<XEv>& stopCriteria) override {
    // XSolution& s = se.first;
    // XEv& e = se.second;
    // Timer tNow;

    unsigned int iter = 0;

    // TODO: de-referentiation of 'target_f' WILL crash, if not provided!!
    // removing 'target_f'
    while ((iter < iterMax) && !stopCriteria.shouldStop(se.second)) {
      // uptr<Move<XES, XSH>> move = ns.randomMove(s);
      uptr<Move<XES, XSH>> move = ns->randomMove(se);

      op<XEv> cost = std::nullopt;

      // if (move && move->canBeApplied(s))
      if (move && move->canBeApplied(se)) {
        cost = evaluator->moveCost(*move, se);
      } else {
        iter++;
        continue;
      }

      iter++;

      // if (cost && evaluator.isImprovement(*cost))
      // if (cost && cost->isImprovingStrict())
      if (cost && evaluator->isStrictImprovement(*cost)) {
        move->applyUpdate(se);
        evaluator->reevaluate(se);
        iter = 0;
      }
    }
    return SearchStatus::NO_REPORT;
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":RDM";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_RANDOMDESCENTMETHOD_HPP_
