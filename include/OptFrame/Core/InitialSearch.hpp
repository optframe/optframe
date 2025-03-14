// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_INITIALSEARCH_HPP_
#define OPTFRAME_INITIALSEARCH_HPP_

#include <string>
#include <utility>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Concepts/Domain.hpp>
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Search/SearchStatus.hpp>
#include <OptFrame/Search/StopCriteria.hpp>

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
class InitialSearch : public Component {
  // PRIMARY/BEST search type is XSH (note that XES is only helper for XSH)
  using XEv = typename XES::second_type;

 public:
  ~InitialSearch() override = default;

  // timelimit in seconds, accepting fractions (millisecs, ...)
  // may or may not generate valid solution in time
  virtual std::pair<std::optional<XSH>, SearchStatus> initialSearch(
      const StopCriteria<XEv>& stop) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":InitialSearch"
       << Domain::getAlternativeDomain<XSH>("<XESf64>");
    // NOTE THAT: PRIMARY/BEST IS TYPICALLY XESf64
    // SECONDARY TYPE IS IGNORED FOR NOW...
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
class BasicInitialSearch : public InitialSearch<XES, XSH> {
  using XEv = typename XES::second_type;

 public:
  using S = typename XES::first_type;

  sref<Constructive<S>> constructive;
  sref<IEvaluator<XES>> evaluator;

  BasicInitialSearch(sref<Constructive<S>> _constructive,
                     sref<IEvaluator<XES>> _evaluator)
      : constructive(_constructive), evaluator(_evaluator) {}

  virtual ~BasicInitialSearch() = default;

  std::pair<std::optional<XSH>, SearchStatus> initialSearch(
      const StopCriteria<XEv>& stop) override {
    if (Component::verbose)
      std::cout << "BasicInitialSearch: initialSearch begins" << std::endl;
    std::optional<S> sol = constructive->generateSolution(stop.timelimit);

    if (Component::verbose)
      std::cout << "BasicInitialSearch: sol? " << (bool)sol << std::endl;

    if (!sol) return make_pair(nullopt, SearchStatus::NO_REPORT);

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    if constexpr (XOStreamable<S>)
      if (Component::verbose)
        std::cout << "BasicInitialSearch: sol => " << *sol << std::endl;
#endif

    XEv e = evaluator->evaluate(*sol);
    XES se(*sol, e);

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    if constexpr (XOStreamable<XES>)
      if (Component::verbose)
        std::cout << "BasicInitialSearch: se => " << se << std::endl;
#endif
    return make_pair(se, SearchStatus::NO_REPORT);
  }
};

}  // namespace optframe

#endif  // OPTFRAME_INITIALSEARCH_HPP_
