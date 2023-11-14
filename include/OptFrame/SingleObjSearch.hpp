// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SINGLEOBJSEARCH_HPP_
#define OPTFRAME_SINGLEOBJSEARCH_HPP_

// C++
#include <iostream>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Constructive.hpp>  // for helper only  (TODO: make special class)
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>  // for helper only (TODO: make special class)
#include <OptFrame/GlobalSearch.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Hyper/Action.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Timer.hpp>

// using namespace std;

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)

template <class Self>
concept
#if __cplusplus <= 201703L  // after c++20, not required 'bool'
    bool
#endif
        XSingleObjSearch = requires(Self a) {
  XESolution<typename Self::BestType>;
};

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
  using XSH = XESS;
  using XEv = typename XESS::second_type;

  // ========================================
  // THIS CLASS IS USELESS! WHAT'S THE POINT?
  // Best to just have XSingleObjSearch
  // ========================================

 public:
  SingleObjSearch() {}

  virtual ~SingleObjSearch() = default;

  // search method try to find a feasible solution within timelimit, if there is
  // no such solution it returns nullptr.

  SearchOutput<XESS> searchBy(const StopCriteria<XEv>& stopCriteria,
                              std::optional<XSH> best) override = 0;

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
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>>
#endif
class SingleObjSearchBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~SingleObjSearchBuilder() = default;

  // NOLINTNEXTLINE
  virtual SingleObjSearch<XES>* build(Scanner& scanner,
                                      HeuristicFactory<S, XEv, XES, X2ES>& hf,
                                      string family = "") = 0;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
    return build(scanner, hf, family);
  }

  vector<pair<string, string>> parameters() override = 0;

  bool canBuild(string) override = 0;

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent();
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
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>>
#endif
class SingleObjSearchAction : public Action<S, XEv, X2ES> {
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
                      HeuristicFactory<S, XEv, XES, X2ES>& hf,
                      map<string, string>& d) {
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
    return ComponentAction<S, XEv>::addAndRegister(scanner, *final, hf, d);
  }

  virtual bool doAction(string content, HeuristicFactory<S, XEv, XES, X2ES>& hf,
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

      S* s;
      hf.assign(s, *scanner.nextInt(), scanner.next());

      if (!scanner.hasNext()) return false;

      XEv* e;
      hf.assign(e, *scanner.nextInt(), scanner.next());

      pair<S, XEv>* p =
          sios->search(StopCriteria<XEv>(timelimit, target_f), s, e);

      if (!p) return true;

      // TODO: use Move Semantics
      S* s2 = new S(p->first);

      delete p;

      return Action<S, XEv, X2ES>::addAndRegister(scanner, *s2, hf, dictionary);
    }

    // no action found!
    return false;
  }
};
}  // namespace optframe

#endif  // OPTFRAME_SINGLEOBJSEARCH_HPP_
