// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_RVND_HPP_
#define OPTFRAME_RVND_HPP_

// =========================================================================
// This is Randomized Variable Neighborhood Descent (RVND)
// A variation of VND proposed in 2010, on paper: A hybrid heuristic
// algorithm for the open-pit-mining operational planning problem
// A similar version has been proposed by A. Subramanian, also in 2010.
// =========================================================================

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <vector>
//
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSEnum.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

#include "FamilyVND.hpp"

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

MOD_EXPORT template <XESolution XES,
                     XEvaluation XEv = typename XES::second_type>
class RVND : public LocalSearch<XES> {
 private:
  sref<GeneralEvaluator<XES>> ev;
  vsref<LocalSearch<XES>> lsList;
  sref<RandGen> rg;

 public:
  RVND(sref<GeneralEvaluator<XES>> _ev, vsref<LocalSearch<XES>> _lsList,
       sref<RandGen> _rg)
      : ev(_ev), lsList(_lsList), rg(_rg) {}

  ~RVND() override = default;

  SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stop) override {
    if (Component::information) std::cout << "RVND::starts" << std::endl;

    Timer tNow;

    rg->shuffle(lsList);  // shuffle elements

    int r = lsList.size();

    int k = 1;

    XES current(se);  // full backup! TODO: remove this copy
    // Evaluation<> eCurrent(e);
    //'target_f' will crash if not provided... removing
    while ((k <= r) &&
           !stop.shouldStop())  // && (ev.betterThan(stopCriteria.target_f, e))
    {
      // eCurrent = e; // backup
      current = se;  // TODO: remove this copy

      StopCriteria<XEv> stopCriteriaNextLS = stop;
      stopCriteriaNextLS.updateTimeLimit(tNow.now());
      lsList[k - 1]->searchFrom(se, stopCriteriaNextLS);

      // if (ev.betterThan(e, eCurrent))
      // if (ev.betterThan(se, current))
      // if (se.second.betterStrict(current.second))
      if (Component::debug)
        std::cout << "RVND will compare(" << se.second.isOutdated() << ";"
                  << current.second.isOutdated() << ")" << std::endl;
      if (ev->betterStrict(se.second, current.second)) {
        // improvement
        k = 1;
        if (Component::information)
          // e.print();
          se.second.print();
      } else {
        k = k + 1;

        if (Component::information) std::cout << "RVND::k=" << k << std::endl;
      }
    }
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":RVND";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override {
    std::stringstream ss;
    ss << "RVND: [ ";
    for (unsigned i = 0; i < lsList.size(); i++) {
      auto& x = const_cast<sref<LocalSearch<XES>>&>(lsList[i]);
      // ss << lsList[i]->toString();
      ss << x->toString();
      if (i != lsList.size() - 1) ss << ",";
    }
    ss << "]";

    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_*/
