// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_LATEACCEPTANCEHILLCLIMBING_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_LATEACCEPTANCEHILLCLIMBING_HPP_

#include <string>
#include <utility>
#include <vector>

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NS.hpp"

#define BRAND_NEW

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
class LateAcceptanceHillClimbing : public LocalSearch<XES> {
  using XEv = typename XES::second_type;

 private:
  sref<GeneralEvaluator<XES>> ev;
  vsref<NS<XES>> lns;
  int L;        // size of list
  int iterMax;  // max iterations without improvement

 public:
  LateAcceptanceHillClimbing(sref<GeneralEvaluator<XES>> _ev, sref<NS<XES>> _ns,
                             int _L, int _iterMax)
      : ev(_ev), L(_L), iterMax(_iterMax) {
    lns.push_back(&_ns);
  }

  LateAcceptanceHillClimbing(sref<GeneralEvaluator<XES>> _ev,
                             vsref<NS<XES>> _lns, int _L, int _iterMax)
      : ev(_ev), lns(_lns), L(_L), iterMax(_iterMax) {}

  virtual ~LateAcceptanceHillClimbing() = default;

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev->evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  SearchStatus searchFrom(XES& seBest, const StopCriteria<XEv>& sosc) override {
    using S = typename XES::first_type;
    // convenient local references on 'seBest'
    S& sStar = seBest.first;
    XEv& eStar = seBest.second;

    double timelimit = sosc.timelimit;

    // XEv target_f(sosc.target_f); // 'target_f' will break... removing

    long tini = time(nullptr);

#ifdef BRAND_NEW
    vector<Evaluation<>*> eList;
    for (int i = 0; i < L; i++) eList.push_back(&eStar.clone());
#else
    vector<double> eList(L, eStar.evaluation());
#endif

    int iter = 1;
    unsigned index = 0;

    S s = sStar;    // copy
    XEv e = eStar;  // copy

    long tnow = time(nullptr);

    while ((iter <= iterMax) &&
           ((tnow - tini) < timelimit))  //&& ev->betterThan(target_f, eStar))
    {
      // choose random neighborhood
      int ns_k = rand() % lns.size();

      uptr<Move<XES, XSH>> move = lns[ns_k]->validRandomMove(seBest);

      if (!move) {
        cout << "Warning in LAHC: cannot find an appliable move for "
                "neighborhood ";
        lns[ns_k]->print();
        // TODO: return FAIL here
      }

      if (move && move->canBeApplied(seBest)) {
        bool mayEstimate = false;
        /// MoveCost<>& cost = *ev->moveCost(*move, se, mayEstimate);
        op<XEv> cost = ev->moveCost(*move, seBest, mayEstimate);

        // test for current index
#ifdef BRAND_NEW
        // if (ev->isImprovement(*cost, e, *eList[index]))
        // if (cost->isImprovingStrict( e, *eList[index]))
        if (ev->isImprovingStrictly(*cost, e, *eList[index]))
#else
        if (ev->betterThan(cost.cost() + e.evaluation(), eList[index]))
#endif
        {
          move->applyUpdate(seBest);
          ev->reevaluate(seBest);

#ifdef BRAND_NEW
          delete eList[index];
          eList[index] = &e.clone();
#else
          eList[index] = e.evaluation();
#endif
          if (ev->betterStrict(e, eStar)) {
            sStar = s;  // copy
            eStar = e;  // copy

            cout << "LAHC: best solution in iter=" << iter << " => ";
            e.print();

            iter = 0;
          }
        }
      }

      iter++;

      index++;
      if (index == eList.size()) index = 0;

      tnow = time(nullptr);
    }

    // delete &e;
    // delete &s;

    // free list

#ifdef BRAND_NEW
    for (unsigned i = 0; i < eList.size(); i++) delete eList[i];
#endif

    eList.clear();
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":LAHC";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }

  std::string toString() const override {
    stringstream ss;
    ss << "LAHC: [ ";
    for (unsigned i = 0; i < lns.size(); i++) {
      ss << lns[i]->toString();
      if (i != lns.size() - 1) ss << ",";
    }
    ss << "]";

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
class LateAcceptanceHillClimbingBuilder
    : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~LateAcceptanceHillClimbingBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner,
                          HeuristicFactory<S, XEv, XES, X2ES>& hf,
                          string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    vsptr<NS<XES>> _nslist;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assignList(_nslist, id2, comp_id2);

    vsref<NS<XES>> nslist;
    for (auto x : _nslist) nslist.push_back(x);

    int L = *scanner.nextInt();

    int iterMax = *scanner.nextInt();

    // NOLINTNEXTLINE
    return new LateAcceptanceHillClimbing<XES>(eval, nslist, L, iterMax);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    stringstream ss;
    ss << NS<XES>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of NS"));
    params.push_back(make_pair("OptFrame:int", "list size L"));
    params.push_back(
        make_pair("OptFrame:int", "iterMax iterations without improvement"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == LateAcceptanceHillClimbing<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << ":LAHC";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_LATEACCEPTANCEHILLCLIMBING_HPP_
