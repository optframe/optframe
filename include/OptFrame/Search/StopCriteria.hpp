// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_STOPCRITERIA_HPP_
#define OPTFRAME_STOPCRITERIA_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <cstring>
#include <functional>  // std::function
#include <iostream>
#include <string>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Search/SearchStatus.hpp>
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

MOD_EXPORT template <XEvaluation XEv = Evaluation<>>
class StopCriteria final : public Component {
 public:
  // NOLINTNEXTLINE
  StopCriteria(std::function<bool(const cop_view<XEv>&)> _stopBy)
      : stopBy(_stopBy) {}

  // NOLINTNEXTLINE
  StopCriteria(double _timelimit = 100000000.0) : timelimit(_timelimit) {}

  StopCriteria(double _timelimit, const XEv& _target_f)
      : timelimit(_timelimit), target_f(_target_f) {}

  ~StopCriteria() override = default;

 public:
  // maximum timelimit (seconds)
  double timelimit = {0};  // disabled -> 0.0

  // maximum number of evaluations
  std::uint64_t maxEvCount = {0};  // disabled -> 0

  // target objective function
  XEv target_f;

  // expected number of Search Space elements
  std::uint64_t xshCount = {0};  // disabled -> 0

  // evaluation counter (starts with zero)
  std::uint64_t evCount = {0};

  // increment evCount
  void incEvaluationCount(std::int64_t v = 1) { evCount += v; }

  // general stopping criteria (method independent)
  std::function<bool(const cop_view<XEv>&)> stopBy{
      [&](const cop_view<XEv>& bestF) -> bool {
        return this->timerExpired() || this->evCountExpired();
      }};

 private:
  //
  // automatic verification of timelimit condition
  Timer localTimer;

 public:
  // now only general stopping criteria supported here
  virtual bool shouldStop(cop_view<XEv> best) const { return stopBy(best); }

  virtual bool shouldStop() const { return stopBy(std::nullopt); }

  // resets timer and returns self-reference
  StopCriteria<XEv>& start() {
    localTimer = Timer();  // reset timer
    return *this;
  }

  // helper method: returns true if timer expired
  bool timerExpired() const {
    return timelimit == 0.0 ? false : getTime() >= timelimit;
  }

  // get spent time
  double getTime() const { return localTimer.now(); }

  // get remaining time (always non-negative or zero if expired)
  double remTime() const {
    double d = timelimit - localTimer.now();
    return (d < 0) ? 0 : d;
  }

  // helper method: returns true if evaluation count expired (TODO: maybe move
  // this to inheritance...)
  bool evCountExpired() const {
    return evCount == 0 ? false : evCount >= maxEvCount;
  }

  void updateTimeLimit(double subtrTL) { timelimit -= subtrTL; }

  StopCriteria<XEv> newStopCriteriaWithTL(double subtrTL) const {
    StopCriteria newStopCriteria = (*this);
    newStopCriteria.timelimit -= subtrTL;
    return newStopCriteria;
  }

  std::string id() const override { return "StopCriteria"; }

  std::string toString() const override { return id(); }
};

template <XESolution XES, XEvaluation XEv, XSearchMethod XM>
using SpecificMethodStop =
    std::function<bool(const XES&, const StopCriteria<XEv>&, XM*)>;
}  // namespace optframe

#endif  // OPTFRAME_STOPCRITERIA_HPP_
