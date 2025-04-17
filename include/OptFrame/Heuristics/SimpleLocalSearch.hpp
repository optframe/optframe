// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_SIMPLELOCALSEARCH_HPP_
#define OPTFRAME_HEURISTICS_SIMPLELOCALSEARCH_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C
#include <math.h>
// C++
#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Search/LocalSearch.hpp>
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

namespace optframe {

template <XESolution XES>
class SimpleLocalSearch : public SingleObjSearch<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 protected:
  sref<Evaluator<S, XEv, XES>> evaluator;
  sref<InitialSearch<XES>> constructive;
  sref<LocalSearch<XES>> localSearch;

 public:
  SimpleLocalSearch(sref<Evaluator<S, XEv, XES>> _evaluator,
                    sref<InitialSearch<XES>> _constructive,
                    sref<LocalSearch<XES>> _localSearch)
      : evaluator(_evaluator),
        constructive(_constructive),
        localSearch(_localSearch) {}

  ~SimpleLocalSearch() override = default;

  // pair<S, Evaluation<>>* search(StopCriteria<XEv>& sosc, const S* _s =
  // nullptr, const Evaluation<>* _e = nullptr) override virtual
  // std::optional<pair<S, XEv>> search(StopCriteria<XEv>& sosc) override
  //
  // SearchStatus search(const StopCriteria<XEv>& sosc) override
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& sosc,
                             std::optional<XES> _best) override {
    // op<XES>& star = this->best;
    //
    op<XES> star;  // TODO: get best from 'searchBy'
    // std::cout << "SimpleLocalSearch search(" << target_f << "," << timelimit
    // <<
    // ")" << std::endl;

    Timer tnow;

    // std::optional<S> s = constructive.generateSolution(sosc.timelimit);
    std::optional<XES> pse = constructive->initialSearch(sosc).first;
    if (!pse) return SearchStatus::NO_SOLUTION;  // nothing to return
    // Evaluation<> e = evaluator.evaluate(*s);

    //// pair<S&, Evaluation<>&>& p = localSearch.search(s, e, sosc);

    // delete &s;

    // return make_optional(std::make_pair(*s, e));
    // star = make_optional(std::make_pair(*s, e));
    star = make_optional(*pse);
    // this->best = star;
    return {SearchStatus::NO_REPORT, star};
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << "SimpleLocalSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  void print() const override {
    std::cout << "SimpleLocalSearch with:" << std::endl;
    std::cout << "constructive: ";
    constructive->print();
    std::cout << "local search: ";
    localSearch->print();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class SimpleLocalSearchBuilder : public SingleObjSearchBuilder<XES> {
  using _S = typename XES::first_type;
  using _XEv = typename XES::second_type;

 public:
  virtual ~SimpleLocalSearchBuilder() {}

  SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                              std::string family = "") override {
    sptr<Evaluator<_S, _XEv, XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<InitialSearch<XES>> constructive;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(constructive, id2, comp_id2);

    std::string rest = scanner.rest();

    std::pair<sptr<LocalSearch<XES>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES>> h = method.first;

    scanner = Scanner(method.second);

    // NOLINTNEXTLINE
    return new SimpleLocalSearch<XES>(eval, constructive, h);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(std::make_pair(Evaluator<_S, _XEv, XES>::idComponent(),
                                    "evaluation function"));
    // params.push_back(std::make_pair(Constructive<S>::idComponent(),
    // "constructive heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == SimpleLocalSearch<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearchBuilder<XES>::idComponent() << "SimpleLocalSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};
}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_SIMPLELOCALSEARCH_HPP_
