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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_HELPER_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_HELPER_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "BaseConcepts.hpp"

#include "Action.hpp"
#include "Evaluation.hpp"
#include "Solution.hpp"
#include "Constructive.hpp" // for helper only  (TODO: make special class)
#include "Evaluator.hpp" // for helper only (TODO: make special class)

#include "Component.hpp"
#include "ComponentBuilder.h"

#include "Timer.hpp" // TODO: move together with StopCriteria to other file!!

#include "GlobalSearch.hpp"

namespace optframe {

template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearchMethod XM = Component>
class SingleObjSearchHelper
{
public:
 
   static std::optional<XES> genPair(Constructive<S>& constructive, Evaluator<XES, XEv>& evaluator, double timelimit, const std::optional<XES> input = std::nullopt)
   {
      // TODO: we must start from InitialSearch generating 'XES'.. thus starting evaluating already, and fully abstracting S.
      // TODO: Evaluator should be GeneralEvaluator... and GeneralEvaluator should only receive XES.. maybe impossible.
      if(input)
         return input;
      std::optional<S> sStar = constructive.generateSolution(timelimit);
      if(!sStar)
         return std::nullopt;
		XEv eStar = evaluator.evaluate(*sStar);
      return make_pair(*sStar, eStar);
   }

   static std::optional<XES> genInitial(InitialSearch<XES, XEv>& constructive, Evaluator<XES, XEv>& evaluator, double timelimit, const std::optional<XES> input = std::nullopt)
   {
      // TODO: we must start from InitialSearch generating 'XES'.. thus starting evaluating already, and fully abstracting S.
      // TODO: Evaluator should be GeneralEvaluator... and GeneralEvaluator should only receive XES.. maybe impossible.
      if(input)
         return input;
      std::optional<S> sStar = constructive.generateSolution(timelimit);
      if(!sStar)
         return std::nullopt;
		XEv eStar = evaluator.evaluate(*sStar);
      return make_pair(*sStar, eStar);
   }
   
};
 
} // namespace optframe

#endif /* OPTFRAME_SINGLE_OBJ_SEARCH_HELPER_HPP_ */
