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

template <XESolution XES>
class SingleObjSearchToLocalSearch : public LocalSearch<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 protected:
  sref<Evaluator<S, XEv, XES>> evaluator;
  sref<SingleObjSearch<XES>> sios;

 public:
  SingleObjSearchToLocalSearch(sref<Evaluator<S, XEv, XES>> _evaluator,
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
    auto& s = se.first;
    auto& e = se.second;

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
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
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

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class SingleObjSearchToLocalSearchBuilder : public LocalSearchBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~SingleObjSearchToLocalSearchBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    sptr<Evaluator<S, XEv, XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    string rest = scanner.rest();

    pair<sptr<SingleObjSearch<XES>>, std::string> method;
    method = hf.createSingleObjSearch(rest);

    sptr<SingleObjSearch<XES>> h = method.first;

    scanner = Scanner(method.second);

    // NOLINTNEXTLINE
    return new SingleObjSearchToLocalSearch<XES>(eval, h);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<S, XEv, XES>::idComponent(),
                               "evaluation function"));
    params.push_back(
        make_pair(SingleObjSearch<XES>::idComponent(), "single obj search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == SingleObjSearchToLocalSearch<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent()
       << "SingleObjSearchToLocalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_SINGLEOBJSEARCHTOLOCALSEARCH_HPP_
