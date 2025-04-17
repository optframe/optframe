// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_GLOBALSEARCH_HPP_
#define OPTFRAME_GLOBALSEARCH_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Core/VEPopulation.hpp>
#include <OptFrame/Search/InitialSearch.hpp>
#include <OptFrame/Search/SearchOutput.hpp>
#include <OptFrame/Search/StopCriteria.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {
// REMEMBER: XES = (some Solution Type, some Evaluation Type)
//
// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
//
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
class GlobalSearch : public Component {
  // not necessarily mono objective XEv... remember this!
  using _XEv = typename XES::second_type;

 public:
  using BestType = XSH;

 public:
  // AVAILABLE HERE
  // best known XSH object: solution/pareto/etc
  // std::optional<XSH> best;
  //
  // NOT AVAILABLE HERE
  // current/working XSH2 object: population/etc
  // std::optional<XSH2> incumbent;
  // ----------
  //
  // callback action 'onBest' is triggered after best is updated (if
  // 'beforeUpdateBest' is required some day, think about it.. not now)
  // returning 'false' should lead to aborting execution (by target, for
  // instance)
  //
  bool (*onBest)(GlobalSearch<XES, BestType>& self, const BestType& best) =
      [](GlobalSearch<XES, BestType>& self, const BestType& best) {
        return true;
      };

  // strict or non-strict search
  bool strict{true};

  GlobalSearch() {}

  ~GlobalSearch() override = default;

  // Assuming method is not thread-safe.
  // Now, we can easily use flag SearchStatus::RUNNING.
  // NOLINTNEXTLINE
  virtual SearchOutput<XES, BestType> search(
      const StopCriteria<_XEv>& stop) final {
    return searchBy(stop, std::nullopt);
  }
  // 'searchBy': Optionally start with some solution (best type XSH)
  // Note this is a copy, not reference, so solution will be moved to the
  // output or even destroyed. SearchOutput MUST hold a solution which is,
  // AT LEAST as good as input 'best', and never worsen it.
  virtual SearchOutput<XES, BestType> searchBy(const StopCriteria<_XEv>& stop,
                                               std::optional<XSH> best) = 0;

  virtual std::string log() const { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":GlobalSearch"

       << Domain::getAlternativeDomain<XES>("<XESf64>");
    // NOTE THAT: PRIMARY/BEST IS TYPICALLY XESf64
    // WE IGNORE SECONDARY TYPE FOR NOW...
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_GLOBALSEARCH_HPP_ // NOLINT
