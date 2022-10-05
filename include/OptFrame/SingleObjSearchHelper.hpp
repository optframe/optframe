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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_HELPER_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_HELPER_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include <OptFrame/BaseConcepts.hpp>

#include "Action.hpp"
#include "Component.hpp"
#include "ComponentBuilder.h"
#include "Constructive.hpp"  // for helper only  (TODO: make special class)
#include "Evaluation.hpp"
#include "Evaluator.hpp"  // for helper only (TODO: make special class)
#include "GlobalSearch.hpp"
#include "Solution.hpp"
#include "Timer.hpp"  // TODO: move together with StopCriteria to other file!!

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>, XSearchMethod XM = Component>
class SingleObjSearchHelper {
 public:
  static std::optional<XES> genPair(Constructive<S>& constructive, Evaluator<XES, XEv>& evaluator, double timelimit, const std::optional<XES> input = std::nullopt) {
    // TODO: we must start from InitialSearch generating 'XES'.. thus starting evaluating already, and fully abstracting S.
    // TODO: Evaluator should be GeneralEvaluator... and GeneralEvaluator should only receive XES.. maybe impossible.
    if (input)
      return input;
    std::optional<S> sStar = constructive.generateSolution(timelimit);
    if (!sStar)
      return std::nullopt;
    XEv eStar = evaluator.evaluate(*sStar);
    return make_pair(*sStar, eStar);
  }

  static std::optional<XES> genInitial(InitialSearch<XES>& constructive, Evaluator<XES, XEv>& evaluator, double timelimit, const std::optional<XES> input = std::nullopt) {
    // TODO: we must start from InitialSearch generating 'XES'.. thus starting evaluating already, and fully abstracting S.
    // TODO: Evaluator should be GeneralEvaluator... and GeneralEvaluator should only receive XES.. maybe impossible.
    if (input)
      return input;
    std::optional<S> sStar = constructive.generateSolution(timelimit);
    if (!sStar)
      return std::nullopt;
    XEv eStar = evaluator.evaluate(*sStar);
    return make_pair(*sStar, eStar);
  }
};

}  // namespace optframe

#endif /* OPTFRAME_SINGLE_OBJ_SEARCH_HELPER_HPP_ */
