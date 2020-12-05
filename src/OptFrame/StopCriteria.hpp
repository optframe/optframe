// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_STOP_CRITERIA_HPP_
#define OPTFRAME_STOP_CRITERIA_HPP_

#include <cstring>
#include <iostream>
#include <vector>

#include <functional> // std::function

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "BaseConcepts.hpp"
#include "SearchStatus.hpp"
#include "Timer.hpp"

using namespace std;

namespace optframe {

// StopCriteria is currently 'final', as lambdas can be passed to specific configurations.
// If something else is required (via inheritance), this 'final' will need to be changed.
//template<XESolution XES, XSearchMethod XM = optframe::Component>
//template<XESolution XES>
template<XEvaluation XEv = Evaluation<>>
class StopCriteria final : public Component
{
public:
   //XM* method { nullptr };

public:
   // maximum timelimit (seconds)
   double timelimit = { 0 }; // disabled -> 0.0
   // 100000000.0

   // maximum number of evaluations
   unsigned long long maxEvCount = { 0 }; // disabled -> 0

   // target objective function
   //double target_f;
   XEv target_f; // TODO (IGOR): pass parameter on SOSC. // TODO: Why?... forgot reason!
   // for MultiObj, 'target_f' can pass ideal values for each objective, and use strict/strong pareto dominance for verification.

   // expected number of Search Space elements
   unsigned long long xshCount = { 0 }; // disabled -> 0
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
      }
   };

private:
   //
   // automatic verification of timelimit condition
   Timer localTimer;
   //
   // evaluation counting can be used this way (to not need inherit this class...):
   //   just pass this object to your Evaluator, and make it 'incEvaluationCount' every evaluation
   unsigned long long evCount = { 0 };

public:
   // increment evCount
   void incEvaluationCount(long long v = 1)
   {
      evCount += v;
   }

   // general stop conditions checked here (does not include best value checking)
   //virtual bool shouldStop(const op<XES>& best, XM* selfMethod) const
   //
   //
   //virtual bool shouldStop(const op<XEv>& best) const
   //
   virtual bool shouldStop(const op<XEv>& best) const
   {
      //return stopBy(best, selfMethod);
      return stopBy(best); // now only general stopping criteria supported here
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
     : stopBy(_stopBy)
   {
   }

   //SOSC(double _timelimit = 100000000.0, double _target_f = 0.0)
   StopCriteria(double _timelimit = 100000000.0)
     //StopCriteria(double _timelimit)
     : timelimit(_timelimit)
   {
   }

   //
   //StopCriteria(double _timelimit, const op<XEv>& _target_f)
   //
   StopCriteria(double _timelimit, const XEv& _target_f)
     : timelimit(_timelimit)
     , target_f(_target_f)
   {
   }

   virtual ~StopCriteria()
   {
   }

   // resets timer and returns self-reference
   StopCriteria<XEv>& start()
   {
      localTimer = Timer(); // reset timer
      return *this;
   }

   // helper method: returns true if timer expired
   bool timerExpired() const
   {
      return timelimit == 0.0 ? false : localTimer.now() >= timelimit;
   }

   // helper method: returns true if evaluation count expired (TODO: maybe move this to inheritance...)
   bool evCountExpired() const
   {
      return evCount == 0 ? false : evCount >= maxEvCount;
   }

   void updateTimeLimit(double subtrTL)
   {
      timelimit -= subtrTL;
   }

   StopCriteria<XEv> newStopCriteriaWithTL(double subtrTL) const
   {
      StopCriteria newStopCriteria = (*this);
      newStopCriteria.timelimit -= subtrTL;
      return newStopCriteria;
   }

   virtual string id() const
   {
      return "StopCriteria";
   }
};

template<XESolution XES, XEvaluation XEv, XSearchMethod XM>
//template<XEvaluation XEv, XSearchMethod XM>
//using SpecificMethodStop = std::function<bool(const op<XEv>&, XM*)>;
using SpecificMethodStop = std::function<bool(const XES&, const StopCriteria<XEv>&, XM*)>; // we guess that method always has a solution to work with

// Default Stop criteria
///using DefaultStop = StopCriteria<Evaluation<>, optframe::Component>;

} // namespace optframe

#endif /* OPTFRAME_STOP_CRITERIA_HPP_ */
