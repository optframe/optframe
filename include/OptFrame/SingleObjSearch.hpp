// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_HPP_

// C++
#include <iostream>
#include <vector>
//
#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/Component.hpp>
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

template <class Self>
concept
#if __cplusplus <= 201703L  // after c++20, not required 'bool'
    bool
#endif
        XSingleObjSearch = requires(Self a) {
  XESolution<typename Self::BestType>;
};

// template<XESolution XES, XEvaluation XEv = Evaluation<>>
// concept SolEv;

// This is a XES, XES global search... using space XES = <S, XEv>
// template<XESolution XES, XSearchMethod XM = Component>
//
// template<XESolution XES, XEvaluation XEv = Evaluation<>>
//
// (Primary) Search space is decided by XES
// Secondary search space XSH2 is undecided... could be trajectory-based (as
// default) or population-based
template <XESolution XES, XESolution XES2 = XES, XSearch<XES2> XSH2 = XES2>
class SingleObjSearch : public GlobalSearch<XES, XES>  // public Component
{
  using XEv = typename XES::second_type;
  // if passing types directly here, error 'typedef declared auto'
  // typedef vector<XEv*> FitnessValues;
  // typedef const vector<const XEv*> ConstFitnessValues;

  using XSH = XES;  // XSearch<XES> = XES

  // onIncumbent for SingleObjSearch
  // bool (*onSingleIncumbent)(GlobalSearch<XES, XSH>& self, const XSH2&
  // incumbent) =
  //  [](GlobalSearch<XES, XSH>& self, const XSH2& incumbent) { return true; };

  // ========================================
  // THIS CLASS IS USELESS! WHAT'S THE POINT?
  // Best to just have XSingleObjSearch
  // ========================================

 public:
  SingleObjSearch() {}

  virtual ~SingleObjSearch() = default;

  // search method try to find a feasible solution within timelimit, if there is
  // no such solution it returns nullptr.
  // virtual pair<S, XEv>* search(StopCriteria<XEv>& stopCriteria, const S* _s =
  // nullptr, const XEv* _e = nullptr) = 0; virtual std::optional<pair<S, XEv>>
  // search(StopCriteria<XEv>& stopCriteria, const std::optional<pair<S, XEv>>
  // input = std::nullopt) = 0;

  // virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria)
  // = 0; virtual SearchStatus search(op<XSH>& inputOutput, const XStop&
  // stopCriteria) = 0; virtual SearchStatus search(op<XSH>& inputOutput, const
  // StopCriteria<XEv>& stopCriteria) = 0;
  //
  //  inputOutput now passed directly on local variable 'this->best'
  //
  // virtual SearchStatus search(const StopCriteria<XEv>& stopCriteria) = 0;
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XSH> best) override = 0;

  std::string log() const override { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (GlobalSearch<XES, XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << GlobalSearch<XES, XES>::idComponent() << ":SingleObjSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }
};

// template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S =
// CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>> template<XESolution XES,
// XEvaluation XEv = Evaluation<>>
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class SingleObjSearchBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~SingleObjSearchBuilder() {}

  virtual SingleObjSearch<XES>* build(Scanner& scanner,
                                      HeuristicFactory<S, XEv, XES, X2ES>& hf,
                                      string family = "") = 0;

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
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class SingleObjSearchAction : public Action<S, XEv, X2ES> {
 public:
  virtual ~SingleObjSearchAction() {}

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

#endif /* OPTFRAME_SINGLE_OBJ_SEARCH_HPP_ */
