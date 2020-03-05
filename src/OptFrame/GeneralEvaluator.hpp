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

#ifndef OPTFRAME_GENERAL_EVALUATOR_HPP_
#define OPTFRAME_GENERAL_EVALUATOR_HPP_

#include "ADSManager.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"
#include "MoveCost.hpp"
#include "Solution.hpp"

#include <iostream>

#include "Action.hpp"
#include "Direction.hpp"

#include "Solutions/ESolution.hpp" // TODO: remove.. just to enforce compilation errors.

// This evaluator intends to run for both Single and MultiObj

using namespace std;
using namespace scannerpp;

namespace optframe {

// This will have no 'weights', as it doesn't make sense for most cases...
// A special WeightedEvaluator is more welcome than putting it on a General one

//template<XSolution S, XEvaluation XEv, XSearch<S, XEv> XSH>
//template<XSolution S, XEvaluation XEv, XESolution XES, XSearch<XES> XSH = XES> // defaults to single obj.
template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES> // defaults to single obj.
// BREAK TIME!! Only 'XES' shall pass!!
class GeneralEvaluator
{
   //using S = decltype(declval<XES>.first); // error: insufficient contextual information to determine type
   //
// could contain Direction here, or MultiDirection... but both should be abolished I guess.
// Where should betterThan decision take place? On Evaluator or on each Evaluation? 
// Before, it was hard/impossible to do on Evaluation, due to overhead... but now, it may be the case.
// If XEvaluation actually represents Objective Space, it should also contain guiding directions.

public:
   GeneralEvaluator()
   {
   }

   virtual ~GeneralEvaluator()
   {
   }

public:
   // general evaluation for Search Space element.
   // now user is free to update evaluation, and even solution during the process.
   // note that intention is to only change evaluation, but now it's up to user.
   virtual void reevaluate(XSH&) = 0;  // TODO: rename to something more general, 'xevaluate' ?

   // Apply movement considering a previous XEv => Faster (used on CheckCommand and locally)
   // Update XEv 'e'
   //Move<XES, XEv, XSH>* applyMoveReevaluate(XEv& e, Move<XES, XEv, XSH>& m, S& s)
   uptr<Move<XES, XEv, XSH>> applyMoveReevaluate(Move<XES, XEv, XSH>& m, XSH& se)
   {
      // apply move and get reverse move
      uptr<Move<XES, XEv, XSH>> rev = m.applyUpdate(se);
      // for now, must be not nullptr
      assert(rev != nullptr);
      // consolidate 'outdated' XEv data on 'e'
      reevaluate(se);

      // create pair
      return rev;
   }

   // TODO: decide which methods stay here, and which go to Evaluator.hpp

};

} // namespace optframe

#endif /*OPTFRAME_GENERAL_EVALUATOR_HPP_*/
