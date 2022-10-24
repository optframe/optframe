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

#ifndef OPTFRAME_STOP_CRITERIA_HPP_
#define OPTFRAME_STOP_CRITERIA_HPP_

#include <cstring>
#include <functional>  // std::function
#include <iostream>
#include <vector>

#include "Component.hpp"
//#include "ComponentBuilder.h"

#include <OptFrame/BaseConcepts.hpp>

#include "SearchStatus.hpp"
#include "Timer.hpp"

using namespace std;

namespace optframe {

// StopCriteria is currently 'final', as lambdas can be passed to specific configurations.
// If something else is required (via inheritance), this 'final' will need to be changed.
//template<XESolution XES, XSearchMethod XM = optframe::Component>
//template<XESolution XES>
template <XEvaluation XEv = Evaluation<>>
class StopCriteria final : public Component {
 public:
  //XM* method { nullptr };

 public:
  // maximum timelimit (seconds)
  double timelimit = {0};  // disabled -> 0.0
  // 100000000.0

  // maximum number of evaluations
  unsigned long long maxEvCount = {0};  // disabled -> 0

  // target objective function
  //double target_f;
  XEv target_f;  // TODO (IGOR): pass parameter on SOSC. // TODO: Why?... forgot reason!
  // for MultiObj, 'target_f' can pass ideal values for each objective, and use strict/strong pareto dominance for verification.

  // expected number of Search Space elements
  unsigned long long xshCount = {0};  // disabled -> 0
  // in case of Population search, is population size; in case of Pareto search, is pareto size.
  // very useful for unified constructive methods, being it single or multiobjective
  // for single objective trajectory search, it is usually ignored, as working number tends to be always one.

  /*
   // general stopping criteria
   std::function<bool(const XES&)> generalStopBy = 
   {   
      [&](const XES& bestF) -> bool {
         return this->timerExpired() || this->evCountExpired();
      }
   };
*/

  // method-specific stopping criteria (default is a generic one)
  /*
   std::function<bool(const op<XES>&, XM*)> stopBy 
   { 
      [&](const op<XES>& bestF, XM*) -> bool {
         return this->timerExpired() || this->evCountExpired();
      }   
   };
   */
  // general stopping criteria (method independent)
  std::function<bool(const op<XEv>&)> stopBy{
      [&](const op<XEv>& bestF) -> bool {
        return this->timerExpired() || this->evCountExpired();
      }};

 private:
  //
  // automatic verification of timelimit condition
  Timer localTimer;
  //
  // evaluation counting can be used this way (to not need inherit this class...):
  //   just pass this object to your Evaluator, and make it 'incEvaluationCount' every evaluation
  unsigned long long evCount = {0};

 public:
  // increment evCount
  void incEvaluationCount(long long v = 1) {
    evCount += v;
  }

  // general stop conditions checked here (does not include best value checking)
  //virtual bool shouldStop(const op<XES>& best, XM* selfMethod) const
  //
  //
  //virtual bool shouldStop(const op<XEv>& best) const
  //

  // TODO: do not use std::optional here... let's use good old XEv* syntax
  // reason: elements are usually paired, so cost to unpair and make_optional at every search cycle can become expensive
  virtual bool shouldStop(const XEv& best) const {
    //return stopBy(best, selfMethod);
    return stopBy(std::make_optional(best));  // now only general stopping criteria supported here
  }

  virtual bool shouldStop() const {
    return stopBy(std::nullopt);
  }

  // ----------------------------------------------
  // method-specific stop criteria (passed by user)
  // ----------------------------------------------
  //StopCriteria(std::function<bool(const op<XES>&, XM*)> _stopBy) :
  //   stopBy(_stopBy), specific(true)
  //{
  //}

  // ---------------------------------
  // general stop criteria (see below)
  // ---------------------------------
  StopCriteria(std::function<bool(const op<XEv>&)> _stopBy)
      : stopBy(_stopBy) {
  }

  //SOSC(double _timelimit = 100000000.0, double _target_f = 0.0)
  StopCriteria(double _timelimit = 100000000.0)
      //StopCriteria(double _timelimit)
      : timelimit(_timelimit) {
  }

  //
  //StopCriteria(double _timelimit, const op<XEv>& _target_f)
  //
  StopCriteria(double _timelimit, const XEv& _target_f)
      : timelimit(_timelimit), target_f(_target_f) {
  }

  virtual ~StopCriteria() {
  }

  // resets timer and returns self-reference
  StopCriteria<XEv>& start() {
    localTimer = Timer();  // reset timer
    return *this;
  }

  // helper method: returns true if timer expired
  bool timerExpired() const {
    return timelimit == 0.0 ? false : getTime() >= timelimit;
  }

  double getTime() const {
    return localTimer.now();
  }

  // helper method: returns true if evaluation count expired (TODO: maybe move this to inheritance...)
  bool evCountExpired() const {
    return evCount == 0 ? false : evCount >= maxEvCount;
  }

  void updateTimeLimit(double subtrTL) {
    timelimit -= subtrTL;
  }

  StopCriteria<XEv> newStopCriteriaWithTL(double subtrTL) const {
    StopCriteria newStopCriteria = (*this);
    newStopCriteria.timelimit -= subtrTL;
    return newStopCriteria;
  }

  std::string id() const override {
    return "StopCriteria";
  }

  virtual std::string toString() const override {
    return id();
  }
};

template <XESolution XES, XEvaluation XEv, XSearchMethod XM>
//template<XEvaluation XEv, XSearchMethod XM>
//using SpecificMethodStop = std::function<bool(const op<XEv>&, XM*)>;
using SpecificMethodStop = std::function<bool(const XES&, const StopCriteria<XEv>&, XM*)>;  // we guess that method always has a solution to work with

// Default Stop criteria
///using DefaultStop = StopCriteria<Evaluation<>, optframe::Component>;

}  // namespace optframe

#endif /* OPTFRAME_STOP_CRITERIA_HPP_ */
