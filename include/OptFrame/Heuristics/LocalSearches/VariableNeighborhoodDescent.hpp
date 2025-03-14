// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_
#define OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_

#include <vector>
//
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSEnum.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

#include "VND.h"

namespace optframe {

// When RandGen is given as parameter it performs RVND
template <XESolution XES, XEvaluation XEv = typename XES::second_type>
class VariableNeighborhoodDescent : public LocalSearch<XES> {
 private:
  sref<GeneralEvaluator<XES>> ev;
  vsref<LocalSearch<XES>> lsList;
  sptr<RandGen> rg;

 public:
  VariableNeighborhoodDescent(sref<GeneralEvaluator<XES>> _ev,
                              vsref<LocalSearch<XES>> _lsList,
                              sptr<RandGen> _rg = nullptr)
      : ev(_ev), lsList(_lsList), rg(_rg) {}

  virtual ~VariableNeighborhoodDescent() {}

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  SearchStatus searchFrom(XES& se,
                          const StopCriteria<XEv>& stopCriteria) override {
    // S& s = se.first;
    // XEv& e = se.second;

    if (Component::information) std::cout << "VND::starts" << std::endl;

    Timer tNow;

    if (rg) rg->shuffle(lsList);  // shuffle elements

    int r = lsList.size();

    int k = 1;

    XES current(se);  // full backup! TODO: remove this copy
    // Evaluation<> eCurrent(e);
    //'target_f' will crash if not provided... removing
    while ((k <= r) &&
           !stopCriteria
                .shouldStop())  // && (ev.betterThan(stopCriteria.target_f, e))
    {
      // eCurrent = e; // backup
      current = se;  // TODO: remove this copy

      StopCriteria<XEv> stopCriteriaNextLS = stopCriteria;
      stopCriteriaNextLS.updateTimeLimit(tNow.now());
      lsList[k - 1]->searchFrom(se, stopCriteriaNextLS);

      // if (ev.betterThan(e, eCurrent))
      // if (ev.betterThan(se, current))
      // if (se.second.betterStrict(current.second))
      if (Component::debug)
        std::cout << "VND will compare(" << se.second.isOutdated() << ";"
                  << current.second.isOutdated() << ")" << std::endl;
      if (ev->betterStrict(se.second, current.second)) {
        // improvement
        k = 1;
        if (Component::information)
          // e.print();
          se.second.print();
      } else {
        k = k + 1;

        if (Component::information) std::cout << "VND::k=" << k << std::endl;
      }
    }
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":VND";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override {
    std::stringstream ss;
    ss << "VND: [ ";
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
