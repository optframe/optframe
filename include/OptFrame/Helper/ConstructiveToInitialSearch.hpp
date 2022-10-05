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

#ifndef OPTFRAME_CONSTRUCTIVE_TO_INITIALSEARCH_HPP_
#define OPTFRAME_CONSTRUCTIVE_TO_INITIALSEARCH_HPP_

#include <OptFrame/BaseConcepts.hpp>

#include "Component.hpp"
//#include "Solution.hpp"
//#include "Solutions/CopySolution.hpp"
#include "InitialSearch.hpp"  // TODO: remove

namespace optframe {

//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R, ADS> S = CopySolution<R, ADS>>
template <XESolution XES>
class ConstructiveToInitialSearch : public InitialSearch<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  Constructive<S>& c;
  Evaluator<S, XEv, XES>& eval;

  ConstructiveToInitialSearch(Constructive<S>& _c, Evaluator<S, XEv, XES>& _eval)
      : c(_c), eval(_eval) {
  }

  virtual ~ConstructiveToInitialSearch() {
  }

  // timelimit in seconds, accepting fractions (millisecs, ...)
  // may or may not generate valid solution in time
  std::pair<std::optional<XES>, SearchStatus> initialSearch(const StopCriteria<XEv>& sosc) override {
    std::optional<S> s = c.generateSolution(sosc.timelimit);
    if (!s)
      return make_pair(nullopt, SearchStatus::NO_REPORT);
    XES se = make_pair(*s, eval.evaluate(*s));
    return make_pair(make_optional(se), SearchStatus::NO_REPORT);
  }

  virtual bool compatible(std::string s) {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":ConstructiveToInitialSearch";
    return ss.str();
  }

  virtual std::string id() const {
    return idComponent();
  }
};
}  // namespace optframe

#endif /*OPTFRAME_CONSTRUCTIVE_TO_INITIALSEARCH_HPP_*/
