// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_LOCALSEARCH_HPP_
#define OPTFRAME_LOCALSEARCH_HPP_

#include <iostream>
#include <string>
#include <utility>
#include <vector>

//
#include "Component.hpp"
#include "Evaluation.hpp"
#include "NSIterator.hpp"
#include "SingleObjSearch.hpp"
//#include "Solution.hpp"
#include <OptFrame/Hyper/ComponentBuilder.hpp>

// using namespace std;

namespace optframe {

// Maybe LocalSearch should only deal with INCUMBENT TYPES...
// That may be a difference from GlobalSearch, that deal with BEST and INCUMBENT
// types...
// TODO: must investigate

// TODO: may pass just XESolution and XEvaluation here (for StopCriteria)... no
// XSolution explicitly required.
template <XESolution XES, XEvaluation XEv = typename XES::second_type,
          XSearch<XES> XSH = XES>  // defaults to XSH = XES
class LocalSearch : public Component {
  typedef vector<XEv*> FitnessValues;
  typedef const vector<const XEv*> ConstFitnessValues;

 public:
  LocalSearch() {
    // DEFAULT for LocalSearch: silent
    Component::setSilent();
  }

  virtual ~LocalSearch() {}

  // core methods

  // copy-based version (TODO: deprecate this?)
  XES lsearch(const XES& se, const StopCriteria<XEv>& stopCriteria) {
    // S& s2 = s.clone();
    // XEv& e2 = e.clone();
    XES p2 = se;  // implicit 'clone' here ??
    searchFrom(p2, stopCriteria);
    // return *new pair<S&, XEv&> (s2, e2);
    return p2;
  }

  // core methods

  // 1
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria) = 0;

  // keeping only this method, for simplification
  // 2
  // virtual void exec(pair<S, XEv>& se, const StopCriteria<XEv>& stopCriteria)
  // = 0;
  virtual SearchStatus searchFrom(XES& se,
                                  const StopCriteria<XEv>& stopCriteria) = 0;

  // optional: set local optimum status (LOS)
  virtual void setLOS(LOS los, string nsid, XES& se) {}

  // optional: get local optimum status (LOS)
  virtual LOS getLOS(string nsid, XES& se) { return los_unknown; }

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
  virtual ~LocalSearchBuilder() {}

  virtual LocalSearch<XES, XEv, XSH>* build(
      Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf,
      string family = "") = 0;

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
