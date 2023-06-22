// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_LOCALSEARCH_HPP_
#define OPTFRAME_LOCALSEARCH_HPP_

#include <iostream>
#include <string>
#include <utility>
#include <vector>

//
#include <OptFrame/Component.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/NSIterator.hpp>
#include <OptFrame/SingleObjSearch.hpp>

// using namespace std;

namespace optframe {

// LocalSearch should only deal with INCUMBENT (secondary) TYPES, what
// differs from GlobalSearch, that manages primary/best types and eventually
// some secondary/incumbent types.

template <XESolution XES2, XSearch<XES2> XSH2 = XES2>
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

  virtual SearchStatus searchFrom(XSH2& se,
                                  const StopCriteria<XEv>& stopCriteria) = 0;

  // optional: set local optimum status (LOS)
  virtual void setLOS(LOS los, string nsid, XSH2& se) {}

  // optional: get local optimum status (LOS)
  virtual LOS getLOS(string nsid, XSH2& se) { return los_unknown; }

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

  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);
    return true;
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = XES>
class LocalSearchBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~LocalSearchBuilder() = default;

  // NOLINTNEXTLINE
  virtual LocalSearch<XES, XSH>* build(Scanner& scanner,
                                       HeuristicFactory<S, XEv, XES, X2ES>& hf,
                                       string family = "") = 0;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
    return build(scanner, hf, family);
  }

  std::vector<pair<string, string>> parameters() override = 0;

  bool canBuild(string) override = 0;

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "LocalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_LOCALSEARCH_HPP_
