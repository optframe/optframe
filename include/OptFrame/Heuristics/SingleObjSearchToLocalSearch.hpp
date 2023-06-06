// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_SINGLEOBJSEARCHTOLOCALSEARCH_HPP_
#define OPTFRAME_HEURISTICS_SINGLEOBJSEARCHTOLOCALSEARCH_HPP_

#include <math.h>

#include <string>
#include <utility>
#include <vector>

#include "../Constructive.hpp"
#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class SingleObjSearchToLocalSearch : public LocalSearch<XES, XEv> {
 protected:
  sref<Evaluator<XES, XEv>> evaluator;
  sref<SingleObjSearch<XES>> sios;

 public:
  SingleObjSearchToLocalSearch(sref<Evaluator<XES, XEv>> _evaluator,
                               sref<SingleObjSearch<XES>> _sios)
      : evaluator(_evaluator), sios(_sios) {}

  virtual ~SingleObjSearchToLocalSearch() {}

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  virtual SearchStatus searchFrom(XES& se,
                                  const StopCriteria<XEv>& sosc) override {
    XSolution AUTO_CONCEPTS& s = se.first;
    XEv& e = se.second;

    // will ignore 'se'
    // maybe ALL SingleObjSearch should inherit from LocalSearch!
    // maybe 'LocalSearch' should become 'Search', and 'SingleObjSearch' ->
    // 'GlobalSearch'... must think!

    std::optional<XES> r = make_optional(se);

    assert(false);
    // TODO: must use 'searchBy'
    //
    // sios.best = r;
    // sios.search(sosc);
    // r = sios.best;

    if (r) {
      s = r->first;
      e = r->second;
      // delete r;  // no need
    }
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << "SingleObjSearchToLocalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual string id() const override { return idComponent(); }

  void print() const override {
    cout << "SingleObjSearchToLocalSearch with sios: ";
    sios->print();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class SingleObjSearchToLocalSearchBuilder
    : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~SingleObjSearchToLocalSearchBuilder() {}

  LocalSearch<XES, XEv>* build(Scanner& scanner,
                               HeuristicFactory<S, XEv, XES, X2ES>& hf,
                               string family = "") override {
    sptr<Evaluator<XES, XEv>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    string rest = scanner.rest();

    pair<sptr<SingleObjSearch<XES>>, std::string> method;
    method = hf.createSingleObjSearch(rest);

    sptr<SingleObjSearch<XES>> h = method.first;

    scanner = Scanner(method.second);

    // NOLINTNEXTLINE
    return new SingleObjSearchToLocalSearch<XES, XEv>(eval, h);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(SingleObjSearch<XES>::idComponent(), "single obj search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == SingleObjSearchToLocalSearch<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent()
       << "SingleObjSearchToLocalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_SINGLEOBJSEARCHTOLOCALSEARCH_HPP_
