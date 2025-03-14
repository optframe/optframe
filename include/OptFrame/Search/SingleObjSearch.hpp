// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SINGLEOBJSEARCH_HPP_
#define OPTFRAME_SINGLEOBJSEARCH_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

// C++
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Core/Constructive.hpp>  // for helper only  (TODO: make special class)
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>  // for helper only (TODO: make special class)
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Hyper/Action.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Search/GlobalSearch.hpp>
#include <OptFrame/Timer.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

// using namespace std;

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

template <class Self>
concept XSingleObjSearch =
    requires(Self a) { requires XESolution<typename Self::BestType>; };

#endif  // cpp_concepts

// REMEMBER: XES = (S, E)
//
// (Primary) Search space (implicit XSH) is decided by XES
// Secondary search space XSH2 is undecided... could be trajectory-based (as
// default) or population-based
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESSolution XESS, XESSolution XESS2 = XESS,
                     XSearch<XESS2> XSH2 = XESS2>
#else
MOD_EXPORT template <typename XESS, typename XESS2 = XESS,
                     typename XSH2 = XESS2>
#endif
class SingleObjSearch : public GlobalSearch<XESS, XESS> {
 public:
  // NOTE THAT: XSearch<XES> XSH = XES (IMPLICIT!)
  using _XSH = XESS;
  using _XEv = typename XESS::second_type;

  // ========================================
  // THIS CLASS IS USELESS! WHAT'S THE POINT?
  // Best to just have XSingleObjSearch
  // ========================================

 public:
  SingleObjSearch() {}

  virtual ~SingleObjSearch() = default;

  // search method try to find a feasible solution within timelimit, if there is
  // no such solution it returns nullptr.

  SearchOutput<XESS> searchBy(const StopCriteria<_XEv>& stopCriteria,
                              std::optional<_XSH> best) override = 0;

  std::string log() const override { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (GlobalSearch<XESS, XESS>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GlobalSearch<XESS, XESS>::idComponent() << ":SingleObjSearch"
       << Domain::getAlternativeDomain<XESS>("<XESf64>");
    // NOTE THAT: PRIMARY/BEST IS ALWAYS XESf64 FOR SINGLE-OBJ-SEARCH
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

}  // namespace optframe

#endif  // OPTFRAME_SINGLEOBJSEARCH_HPP_
