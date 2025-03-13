// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SINGLEOBJSEARCH_HPP_
#define OPTFRAME_SINGLEOBJSEARCH_HPP_

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
#include <OptFrame/GlobalSearch.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Hyper/Action.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Timer.hpp>

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
template <XESSolution XESS, XESSolution XESS2 = XESS,
          XSearch<XESS2> XSH2 = XESS2>
#else
template <typename XESS, typename XESS2 = XESS, typename XSH2 = XESS2>
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

  static string idComponent() {
    stringstream ss;
    ss << GlobalSearch<XESS, XESS>::idComponent() << ":SingleObjSearch"
       << Domain::getAlternativeDomain<XESS>("<XESf64>");
    // NOTE THAT: PRIMARY/BEST IS ALWAYS XESf64 FOR SINGLE-OBJ-SEARCH
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESSolution XESS>  // single objective type XESSOlution
#else
template <typename XESS>
#endif
class SingleObjSearchBuilder : public ComponentBuilder<XESS> {
  using _XES = XESS;  // single objective type XESSOlution

 public:
  virtual ~SingleObjSearchBuilder() = default;

  // NOLINTNEXTLINE
  virtual SingleObjSearch<_XES>* build(Scanner& scanner,
                                       HeuristicFactory<_XES>& hf,
                                       string family = "") = 0;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner, HeuristicFactory<_XES>& hf,
                            string family = "") override {
    return build(scanner, hf, family);
  }

  vector<pair<string, string>> parameters() override = 0;

  bool canBuild(string) override = 0;

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<_XES>::idComponent();
    ss << "SingleObjSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

// template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S =
// CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>> template<XESolution XES,
// XEvaluation XEv = Evaluation<>>
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class SingleObjSearchAction : public Action<XES> {
  using _S = typename XES::first_type;
  using _XEv = typename XES::second_type;

 public:
  virtual ~SingleObjSearchAction() = default;

  virtual string usage() {
    return "OptFrame:SingleObjSearch idx   search    timelimit  target_f  "
           "OptFrame:Solution idx|-1   OptFrame:Evaluation idx|-1   "
           "[output_variable] => OptFrame:Solution|nullptr";
  }

  virtual bool handleComponent(string type) {
    return ComponentHelper::compareBase(SingleObjSearch<XES>::idComponent(),
                                        type);
  }

  virtual bool handleComponent(Component& component) {
    return component.compatible(SingleObjSearch<XES>::idComponent());
  }

  virtual bool handleAction(string action) { return (action == "search"); }

  virtual bool doCast(string component, int id, string type, string variable,
                      HeuristicFactory<XES>& hf, map<string, string>& d) {
    if (!handleComponent(type)) {
      cout << "SingleObjSearchAction::doCast error: can't handle component "
              "type '"
           << type << " " << id << "'" << endl;
      return false;
    }

    Component* comp = hf.components[component].at(id);

    if (!comp) {
      cout << "SingleObjSearchAction::doCast error: nullptr component '"
           << component << " " << id << "'" << endl;
      return false;
    }

    if (!ComponentHelper::compareBase(comp->id(), type)) {
      cout << "SingleObjSearchAction::doCast error: component '" << comp->id()
           << " is not base of " << type << "'" << endl;
      return false;
    }

    // remove old component from factory
    hf.components[component].at(id) = nullptr;

    // cast object to lower type
    Component* final = nullptr;

    if (type == SingleObjSearch<XES>::idComponent()) {
      final = (SingleObjSearch<XES>*)comp;
    } else {
      cout << "SingleObjSearchAction::doCast error: no cast for type '" << type
           << "'" << endl;
      return false;
    }

    // add new component
    Scanner scanner(variable);
    return ComponentAction<XES>::addAndRegister(scanner, *final, hf, d);
  }

  virtual bool doAction(string content, HeuristicFactory<XES>& hf,
                        map<string, string>& dictionary,
                        map<string, vector<string>>& ldictionary) {
    // cout << "LocalSearch::doAction '" << content << "'" << endl;

    Scanner scanner(content);

    if (!scanner.hasNext()) return false;

    SingleObjSearch<XES>* sios;
    hf.assign(sios, *scanner.nextInt(), scanner.next());

    if (!sios) return false;

    if (!scanner.hasNext()) return false;

    string action = scanner.next();

    if (!handleAction(action)) return false;

    if (action == "search") {
      if (!scanner.hasNext()) return false;

      double timelimit = *scanner.nextDouble();

      if (!scanner.hasNext()) return false;

      double target_f = *scanner.nextDouble();

      if (!scanner.hasNext()) return false;

      _S* s;
      hf.assign(s, *scanner.nextInt(), scanner.next());

      if (!scanner.hasNext()) return false;

      _XEv* e;
      hf.assign(e, *scanner.nextInt(), scanner.next());

      std::pair<_S, _XEv>* p =
          sios->search(StopCriteria<_XEv>(timelimit, target_f), s, e);

      if (!p) return true;

      // TODO: use Move Semantics
      _S* s2 = new _S(p->first);

      delete p;

      return Action<XES>::addAndRegister(scanner, *s2, hf, dictionary);
    }

    // no action found!
    return false;
  }
};
}  // namespace optframe

#endif  // OPTFRAME_SINGLEOBJSEARCH_HPP_
