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

#ifndef OPTFRAME_INITIAL_SEARCH_HPP_
#define OPTFRAME_INITIAL_SEARCH_HPP_

#include <utility>
//
#include <OptFrame/BaseConcepts.hpp>

#include "Component.hpp"
#include "Constructive.hpp"
#include "Evaluator.hpp"
#include "SearchStatus.hpp"
#include "StopCriteria.hpp"

namespace optframe {

//template<XESolution XES, XSearch<XES> XSH = XES, XSearchMethod XM = Component> // defaults to single obj.
template <XESolution XES, XSearch<XES> XSH = XES>
class InitialSearch : public Component {
  using XEv = typename XES::second_type;

 public:
  virtual ~InitialSearch() {
  }

  // timelimit in seconds, accepting fractions (millisecs, ...)
  // may or may not generate valid solution in time
  virtual std::pair<
      std::optional<XSH>,
      SearchStatus>
  initialSearch(const StopCriteria<XEv>& stop) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":InitialSearch";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }

  std::string toString() const override {
    return id();
  }
};

template <XESolution XES, XSearch<XES> XSH = XES>
class BasicInitialSearch : public InitialSearch<XES, XSH> {
  using XEv = typename XES::second_type;

 public:
  using S = typename XES::first_type;

  sref<Constructive<S>> constructive;
  sref<IEvaluator<XES>> evaluator;

  BasicInitialSearch(sref<Constructive<S>> _constructive,
                     sref<IEvaluator<XES>> _evaluator)
      : constructive(_constructive), evaluator(_evaluator) {
  }

  virtual ~BasicInitialSearch() {
  }

  virtual std::pair<
      std::optional<XSH>,
      SearchStatus>
  initialSearch(const StopCriteria<XEv>& stop) override {
    if (Component::verbose)
      std::cout << "BasicInitialSearch: initialSearch begins" << std::endl;
    std::optional<S> sol = constructive->generateSolution(stop.timelimit);

    if (Component::verbose)
      std::cout << "BasicInitialSearch: sol? " << (bool)sol << std::endl;

    if (!sol)
      return make_pair(nullopt, SearchStatus::NO_REPORT);

    if (Component::verbose)
      std::cout << "BasicInitialSearch: sol => " << *sol << std::endl;

    XEv e = evaluator->evaluate(*sol);
    XES se(*sol, e);

    if (Component::verbose)
      std::cout << "BasicInitialSearch: se => " << se << std::endl;
    return make_pair(se, SearchStatus::NO_REPORT);
  }
};

}  // namespace optframe

#endif  // OPTFRAME_INITIALSEARCH_HPP_
