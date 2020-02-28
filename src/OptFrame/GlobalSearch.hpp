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

#ifndef OPTFRAME_GLOBAL_SEARCH_HPP_
#define OPTFRAME_GLOBAL_SEARCH_HPP_


#include <iostream>
#include <vector>
#include <cstring>

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "BaseConcepts.hpp"

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
   // TODO: create 'INFEASIBLE' or 'IMPOSSIBLE' output here, when problem is invalid.

   // improvement flag
   IMPROVEMENT = 0x02,
   // stop criteria (general)
   STOP_BY_TIME = 0x04,  // stop by timelimit
   STOP_BY_TARGET = 0x08, // stop by target value
   STOP_BY_PARAM = 0x10, // stop by specific method parameter
   // TODO: think about renaming this to 'STOP_BY_OTHER' or 'STOP_BY_LOG'... and inform only on logs what caused this stop (it could be many options...)

   //STOP_BY_MEMORY = 0x20, // stop by memory (useful for branch&bound and other enumeration techniques)

   // other flags (OPTIMALITY CONDITIONS)
   // local optima
   // global optima
   // ... LOWER_BOUND / UPPER_BOUND ? How to allow represent only evaluation and not include solution? TODO (IGOR): think... maybe some specific BoundSearch or RelaxationSearch ??.. 
   // ... sometimes, good also to result solution, and approximation value provided (ApproxSearch) approximative search (route + value + ~1.5 bound)
   // ... finally, maybe good to add Bound to input/output search format... return bound value or approximation in other param (with empty solution)
   LOCAL_OPT = 0x40, // local optimum
   GLOBAL_OPT = 0x80 // global optimum
};



// TODO: move to self file
template<XEvaluation XEv = Evaluation<>>
//class StopCriteria final : public Component
class StopCriteria : public Component // TODO: non-final unless proven necessary by performance
{
public:
   // maximum timelimit (seconds)
   double timelimit = {0}; // disabled -> 0.0
   // maximum number of evaluations
   unsigned long long maxEvCount = {0}; // disabled -> 0

   // target objective function
   //double target_f;
   XEv target_f; // TODO (IGOR): pass parameter on SOSC. // TODO: Why?... forgot reason!
   // for MultiObj, 'target_f' can pass ideal values for each objective, and use strict/strong pareto dominance for verification.

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

   // note 'virtual' ... (only if 'final' is not needed. TODO: check performance)
   // general stop conditions checked here (does not include best value checking)
   virtual bool shouldStop() const 
   {
      // note that this method does not check best value...
      // 'target_f' strategy must be handled directly by specific Evaluator classes on search methods
      return timerExpired() || evCountExpired();
   }

   //SOSC(double _timelimit = 100000000.0, double _target_f = 0.0)
   StopCriteria(double _timelimit = 100000000.0)
     : timelimit(_timelimit)
   {
   }

   StopCriteria(double _timelimit, const XEv& _target_f)
     : timelimit(_timelimit),
     target_f(_target_f)
   {
   }

   StopCriteria(double _timelimit, XEv&& _target_f)
     : timelimit(_timelimit),
     target_f(std::move(_target_f))
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

template<XSolution S, XEvaluation XEv, XSearchSpace<S, XEv> XSearch>
class GlobalSearch: public Component
{
public:

	GlobalSearch()
	{
	}

	virtual ~GlobalSearch()
	{
	}

   virtual SearchStatus search(std::optional<XSearch>& p, const StopCriteria<>& stopCriteria) = 0;

	virtual string log() const
	{
		return "Empty heuristic log.";
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << "GlobalSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<XSolution S, XEvaluation XEv, XSearchSpace<S, XEv> XSearch>
class GlobalSearchBuilder: public ComponentBuilder<S, XEv, XSearch>
{
public:
	virtual ~GlobalSearchBuilder()
	{
	}

	virtual GlobalSearch<S, XEv, XSearch>* build(Scanner& scanner, HeuristicFactory<S, XEv, XSearch>& hf, string family = "") = 0;

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XSearch>& hf, string family = "")
	{
		return build(scanner, hf, family);
	}

	virtual vector<pair<string, string> > parameters() = 0;

	virtual bool canBuild(string) = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, XSearch>::idComponent() << "GlobalSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

} // namespace optframe

#endif /* OPTFRAME_GLOBAL_SEARCH_HPP_ */
