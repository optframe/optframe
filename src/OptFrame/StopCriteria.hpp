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


#include <iostream>
#include <vector>
#include <cstring>

#include<functional> // std::function

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "BaseConcepts.hpp"
#include "Timer.hpp"

using namespace std;

namespace optframe
{

enum class SearchStatus : int 
{
   // better to keep a single flag '0x00' that indicates nothing good happened (no evolution)
   // it means: no improvement on current solution or no valid/feasible solution found
   //NO_CHANGE = 0x00, 
   NO_NEWSOL = 0x00, // no (new) solution found on search
   // TODO: rename to UNCHANGED (?). 'NO_CHANGE' was already bad... 'NO_PROGRESS' (?) maybe.

   // general flag
   VALID_SOL  = 0x01,  // VALID_SOLUTION (?) nothing interesting to say...
   // TODO: rename 'VALID_SOL' to 'HEURISTIC' (general return, since all working solutions are valid).
   //          Better than 'APPROXIMATED' naming, which conflicts to 'APPROXIMATIVE'.
   // improvement flag
   IMPROVEMENT = 0x02,
   // stop criteria (general)
   STOP_BY_TIME = 0x04,  // stop by timelimit
   STOP_BY_OTHER = 0x08, // stop by specific method parameter (target, evaluation count, population count, ..., view logs)
   //STOP_BY_TARGET = 0x10, // stop by target value
   //STOP_BY_MEMORY = 0x10, // stop by memory (useful for branch&bound and other enumeration techniques)

   // other flags (SEARCH TERMINATION CONDITIONS)
   // problem is infeasible (or unbounded)
   IMPOSSIBLE = 0x20, // TODO: rename to 'INFEASIBLE' (?)
   // local optima
   // global optima
   // ... LOWER_BOUND / UPPER_BOUND ? How to allow represent only evaluation and not include solution? TODO (IGOR): think... maybe some specific BoundSearch or RelaxationSearch ??.. 
   // ... sometimes, good also to result solution, and approximation value provided (ApproxSearch) approximative search (route + value + ~1.5 bound)
   // ... finally, maybe good to add Bound to input/output search format... return bound value or approximation in other param (with empty solution)
   LOCAL_OPT = 0x40, // local optimum
   GLOBAL_OPT = 0x80 // global optimum
};



// StopCriteria is currently 'final', as lambdas can be passed to specific configurations. 
// If something else is required (via inheritance), this 'final' will need to be changed.
template<XESolution XES, XSearchMethod XM = optframe::Component> 
class StopCriteria final : public Component
{
public:
   //XM* method { nullptr };

public:
   // maximum timelimit (seconds)
   double timelimit = {0}; // disabled -> 0.0
   // 100000000.0
   
   // maximum number of evaluations
   unsigned long long maxEvCount = {0}; // disabled -> 0

   // target objective function
   //double target_f;
   op<XES> target_f; // TODO (IGOR): pass parameter on SOSC. // TODO: Why?... forgot reason!
   // for MultiObj, 'target_f' can pass ideal values for each objective, and use strict/strong pareto dominance for verification.

   // expected number of Search Space elements
   unsigned long long xshCount = {0}; // disabled -> 0
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
   std::function<bool(const op<XES>&, XM*)> stopBy 
   { 
      [&](const op<XES>& bestF, XM*) -> bool {
         return this->timerExpired() || this->evCountExpired();
      }   
   };

   // indicates if stop function is standard/general or specific (including method params)
   bool specific = { false };

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
   void incEvaluationCount(long long v = 1) 
   {
      evCount += v;
   }

   // general stop conditions checked here (does not include best value checking)
   virtual bool shouldStop(const op<XES>& best, XM* selfMethod) const
   {
      return stopBy(best, selfMethod);
   }

   // ----------------------------------------------
   // method-specific stop criteria (passed by user)
   // ----------------------------------------------

   StopCriteria(std::function<bool(const op<XES>&, XM*)> _stopBy) :
      stopBy(_stopBy), specific(true)
   {
   }

   // ---------------------------------
   // general stop criteria (see below)
   // ---------------------------------

   //SOSC(double _timelimit = 100000000.0, double _target_f = 0.0)
   StopCriteria(double _timelimit = 100000000.0)
   //StopCriteria(double _timelimit)
     : timelimit(_timelimit)
   {
   }

   StopCriteria(double _timelimit, const op<XES>& _target_f)
     : timelimit(_timelimit),
     target_f(_target_f)
   {
   }

   virtual ~StopCriteria()
   {
   }

   // resets timer and returns self-reference
   StopCriteria<XES>& start()
   {
      localTimer = Timer(); // reset timer
      return *this;
   }

   // helper method: returns true if timer expired
   bool timerExpired() const
   {
      return timelimit==0.0 ? false : localTimer.now() >= timelimit;
   }

   // helper method: returns true if evaluation count expired (TODO: maybe move this to inheritance...)
   bool evCountExpired() const
   {
      return evCount==0 ? false : evCount >= maxEvCount;
   }

   void updateTimeLimit(double subtrTL)
   {
      timelimit -= subtrTL;
   }

   StopCriteria<XES> newStopCriteriaWithTL(double subtrTL) const
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

// Default Stop criteria
///using DefaultStop = StopCriteria<Evaluation<>, optframe::Component>;

} // namespace optframe

#endif /* OPTFRAME_STOP_CRITERIA_HPP_ */
