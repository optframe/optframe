// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_LOCALSEARCH_HPP_
#define OPTFRAME_LOCALSEARCH_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <iostream>
#include <string>
#include <utility>
#include <vector>

//
#include <OptFrame/Component.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/NSIterator.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>

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

// using namespace std;

namespace optframe {

// LocalSearch should only deal with INCUMBENT (secondary) TYPES, what
// differs from GlobalSearch, that manages primary/best types and eventually
// some secondary/incumbent types.

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES2, XSearch<XES2> XSH2 = XES2>
#else
MOD_EXPORT template <typename XES2, typename XSH2 = XES2>
#endif
class LocalSearch : public Component {
 public:
  using IncumbentType = XSH2;  // defaults to XES
  using XEv = typename XES2::second_type;

  LocalSearch() {
    // DEFAULT for LocalSearch: silent
    Component::setSilent();
  }

  ~LocalSearch() override = default;

  // core methods

  // copy-based version
  XSH2 lsearch(const XSH2& se, const StopCriteria<XEv>& stopCriteria) {
    XSH2 p2 = se;
    searchFrom(p2, stopCriteria);
    return p2;
  }

  virtual SearchStatus searchFrom(XSH2& se, const StopCriteria<XEv>& stop) = 0;

  // callback types
  using OnLocalOptimumType = std::function<SearchStatus(
      LocalSearch<XES2, XSH2>&, SearchStatus, XSH2&, const StopCriteria<XEv>&)>;
  using OnFinishLocalType = std::function<SearchStatus(
      LocalSearch<XES2, XSH2>&, SearchStatus, XSH2&, const StopCriteria<XEv>&)>;

  // onFinish should run ALWAYS before returning!
  // Useful for flagging LOS status directly on solution!
  // Currently invoked by onLocalOptimum()!
  OnLocalOptimumType onLocalOptimum =
      [](LocalSearch<XES2, XSH2>& self, SearchStatus st, XSH2& se,
         const StopCriteria<XEv>& stop) { return st; };

  // onFinish should run ALWAYS before returning!
  // Useful for post-processing!
  // Currently used to invoke onLocalOptimum()!
  OnFinishLocalType onFinishLocal = [](LocalSearch<XES2, XSH2>& self,
                                       const SearchStatus st, XSH2& se,
                                       const StopCriteria<XEv>& stop) {
    if (st == SearchStatus::LOCAL_OPT)
      return self.onLocalOptimum(self, st, se, stop);
    else
      return st;
  };

  // ====================================================================
  // LOS IS A BAD IDEA AFTERALL... Use onLocalOptimum() Callback instead!
  // ====================================================================

  // optional: set local optimum status (LOS)
  virtual void setLOS(LOS los, std::string nsid, XSH2& se) {}

  // optional: get local optimum status (LOS)
  virtual LOS getLOS(std::string nsid, XSH2& se) { return los_unknown; }

  // ====================================================================

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":LocalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }

  bool setMessageLevelR(modlog::LogLevel ll) override {
    this->setMessageLevel(ll);
    return true;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_LOCALSEARCH_HPP_
