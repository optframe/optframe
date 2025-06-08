// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_EMPTY_LOCALSEARCH_HPP_
#define OPTFRAME_EMPTY_LOCALSEARCH_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <vector>
//
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

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

MOD_EXPORT template <XESolution XES>
class EmptyLS : public LocalSearch<XES> {
  using XEv = typename XES::second_type;

 public:
  EmptyLS() {}

  ~EmptyLS() override = default;

  SearchStatus searchFrom(XES&, const StopCriteria<XEv>& stop) override {
    // placeholder for empty local search
    return SearchStatus::NO_REPORT;
  };

  std::string log() const { return "Heuristic Empty: no log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":EmptyLS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_EMPTY_LOCALSEARCH_HPP_*/
