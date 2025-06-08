// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_HILLCLIMBING_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_HILLCLIMBING_HPP_

// =========================================================================
// This is Hill Climbing (HC)
// A classic neighborhood exploration technique, typically used with BI
//   to perform Steepest Descent.
// Reference paper: unknown (classic)
// =========================================================================

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

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

MOD_EXPORT template <XESolution XES, XEvaluation XEv = Evaluation<>>
class HC : public LocalSearch<XES> {
 private:
  sref<GeneralEvaluator<XES>> evaluator;
  sref<LocalSearch<XES>> ls;

 public:
  HC(sref<GeneralEvaluator<XES>> _ev, sref<LocalSearch<XES>> _ls)
      : evaluator(_ev), ls(_ls) {}

  ~HC() override = default;

  SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stop) override {
    if (Component::information)
      std::cout << "HC (ls=" << ls->toString() << ") starts." << std::endl;
    // S& s = se.first;
    XEv& e = se.second;
    //
    // double timelimit = sosc.timelimit;
    // long tini = time(nullptr);

    // Evaluation<>* e0 = &e.clone();
    XEv e0(e);  // avoid that using return status on 'exec'

    if (Component::information)
      std::cout << "HC invoking ls=" << ls->toString() << std::endl;
    ls->searchFrom(se, stop);

    // long tnow = time(nullptr);

    // while improvement is found
    // while ((evaluator.betterThan(e, e0)) && ((tnow - tini) < timelimit))
    // while ((e.betterStrict(e0)) && ((tnow - tini) < timelimit))
    while ((evaluator->betterStrict(e, e0)) &&
           !stop.shouldStop(e)) {  // ((tnow - tini) < timelimit)) {
      // delete e0;
      // e0 = &e.clone();

      //   (*e0) = e;
      e0 = e;

      ls->searchFrom(se, stop);

      // tnow = time(nullptr);
    }

    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":HC";
    return ss.str();
  }

  bool setMessageLevelR(modlog::LogLevel ll) override {
    this->setMessageLevel(ll);
    // evaluator->setVerboseR();
    return ls->setMessageLevelR(ll);
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_HILLCLIMBING_HPP_
