// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_COMPARELOCALSEARCH_HPP_
#define OPTFRAME_HEURISTICS_COMPARELOCALSEARCH_HPP_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../LocalSearch.hpp"
#include "../NSSeq.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class CompareLocalSearch : public LocalSearch<XES> {
 private:
  sref<Evaluator<XES, XEv>> eval;
  sref<LocalSearch<XES>> ls1;
  sref<LocalSearch<XES>> ls2;

 public:
  CompareLocalSearch(sref<Evaluator<XES, XEv>> _eval,
                     sref<LocalSearch<XES>> _ls1, sref<LocalSearch<XES>> _ls2)
      : eval(_eval), ls1(_ls1), ls2(_ls2) {}

  virtual ~CompareLocalSearch() {}

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& sosc) override {
    // S& s = se.first;
    XEv& e = se.second;

    // S& s2 = s.clone();
    // Evaluation<>& e2   = e.clone();
    XES p2 = se;  // clone!
    XEv& e2 = p2.second;

    ls1->searchFrom(se, sosc);
    ls2->searchFrom(p2, sosc);

    if (!eval->equals(e, e2)) {
      cout << "CompareLocalSearch error: difference between " << e.evaluation()
           << " and " << e2.evaluation() << endl;
      cout << "LocalSearch 1: ";
      ls1->print();
      cout << "LocalSearch 2: ";
      ls2->print();
      exit(1);
    }

    // delete &s2;
    // delete &e2;
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << "CompareLocalSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  string toString() const override {
    stringstream ss;
    ss << "CLS: (" << ls1->toString() << "," << ls2->toString() << ")";
    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>>
#endif
class CompareLocalSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~CompareLocalSearchBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner,
                          HeuristicFactory<S, XEv, XES, X2ES>& hf,
                          string family = "") override {
    std::shared_ptr<Evaluator<XES, XEv>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES>> h = method.first;

    scanner = Scanner(method.second);

    string rest2 = scanner.rest();

    pair<sptr<LocalSearch<XES>>, std::string> method2;
    method2 = hf.createLocalSearch(rest2);

    sptr<LocalSearch<XES>> h2 = method2.first;

    scanner = Scanner(method2.second);

    // NOLINTNEXTLINE
    return new CompareLocalSearch<XES>(eval, h, h2);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search 1"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search 2"));

    return params;
  }

  bool canBuild(string component) override {
    return component == CompareLocalSearch<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << "CompareLocalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_COMPARELOCALSEARCH_HPP_
