// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_GRASP_BASICGRASP_HPP_
#define OPTFRAME_HEURISTICS_GRASP_BASICGRASP_HPP_
// C
// C++
#include <string>
#include <utility>
#include <vector>
//
#include "../../LocalSearch.hpp"
#include "../../SingleObjSearch.hpp"
#include "../../Timer.hpp"
#include "GRASPFamily.h"
#include "GRConstructive.hpp"

namespace optframe {

// GRASP requires S space interaction
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class BasicGRASP : public SingleObjSearch<XES>, public GRASP {
 private:
  sref<GeneralEvaluator<XES>> evaluator;
  sref<GRConstructive<S>> constructive;
  sref<LocalSearch<XES>> ls;
  float alpha;
  unsigned int iterMax;

 public:
  BasicGRASP(sref<GeneralEvaluator<XES>> _eval,
             sref<GRConstructive<S>> _constructive, sref<LocalSearch<XES>> _ls,
             float _alpha, int _iterMax)
      : evaluator(_eval), constructive(_constructive), ls(_ls) {
    if (_iterMax <= 0) _iterMax = 1;
    if (_alpha < 0 || _alpha > 1) _alpha = 1;
    alpha = _alpha;
    iterMax = _iterMax;
  }

  ~BasicGRASP() override = default;

  // TODO: consider input and optional output
  XES genGRPair(double timelimit) {
    std::optional<S> sStar = constructive->generateGRSolution(alpha, timelimit);
    // XEv eStar = evaluator.evaluate(*sStar); // TODO: evaluate should receive
    // S... and reevaluate 'both'
    XEv eStar;
    XES p = make_pair(*sStar, eStar);
    evaluator->reevaluate(p);

    return p;
    // return make_pair(*sStar, eStar); // TODO: generalize this
  }

  // pair<S, Evaluation<>>* search(StopCriteria<XEv>& stopCriteria, const S* _s
  // = nullptr, const Evaluation<>* _e = nullptr) override virtual
  // std::optional<pair<S, XEv>> search(StopCriteria<XEv>& stopCriteria)
  // override
  //
  // SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XES> _best) override {
    // op<XES>& star = this->best;
    op<XES> star;  // TODO: get on 'searchBy'

    double timelimit = stopCriteria.timelimit;

    // XEv target_f(stopCriteria.target_f); // BROKEN!

    Timer tNow;

    unsigned int iter = 0;

    ////S* s = constructive.generateGRSolution(alpha, timelimit);
    XES se = genGRPair(timelimit);
    ////Evaluation<> e = evaluator.evaluate(*s);
    // S& s = se.first;
    XEv& e = se.second;

    if (Component::information) e.print();

    // TODO: use 'stop'
    while ((iter < iterMax) &&
           (tNow.now() < timelimit))  //&& (evaluator.betterThan(target_f, e)))
    {
      if (Component::verbose) cout << "GRASP::iter=" << iter << endl;

      XES p1 = genGRPair(timelimit - tNow.now());
      ////S* s1 = constructive.generateGRSolution(alpha,timelimit - tNow.now());
      ////Evaluation<> e1 = evaluator.evaluate(*s1);
      // S& s1 = p1.first;
      XEv& e1 = p1.second;

      StopCriteria<XEv> stopCriteriaLS =
          stopCriteria.newStopCriteriaWithTL(tNow.now());
      ls->searchFrom(p1, stopCriteriaLS);

      // if (evaluator.betterThan(e1, e))
      // if (e1.betterStrict(e))
      if (evaluator->betterStrict(e1, e)) {
        //(*s) = std::move(*s1);
        // delete s1;
        // e = std::move(e1);
        se = p1;  // TODO: better move perhaps??

        if (Component::information) {
          cout << "GRASP iter " << iter << ": ";
          e.print();
        }
      }

      iter++;
    }
    // S sFinal = std::move(*s);
    // delete s;

    // return new pair<S, Evaluation<>>(sFinal, e);
    // return make_pair(make_optional(se), SearchStatus::NO_REPORT);
    star = make_optional(se);
    //
    // this->best = star;
    return {SearchStatus::NO_REPORT, star};
  }

  string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":" << GRASP::family()
       << ":BasicGRASP";
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicGRASPBuilder : public GRASP,
                          public SingleObjSearchBuilder<S, XEv, XES> {
 public:
  ~BasicGRASPBuilder() override = default;

  SingleObjSearch<XES>* build(Scanner& scanner,
                              HeuristicFactory<S, XEv, XES, X2ES>& hf,
                              string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<GRConstructive<S>> constructive;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(constructive, id2, comp_id2);

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES>> h = method.first;

    scanner = Scanner(method.second);

    if (!scanner.hasNext()) return nullptr;

    float alpha = *scanner.nextFloat();

    if (!scanner.hasNext()) return nullptr;

    int iterMax = *scanner.nextInt();

    // NOLINTNEXTLINE
    return new BasicGRASP<S, XEv>(eval, constructive, h, alpha, iterMax);
  }

  vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(make_pair(GRConstructive<S>::idComponent(),
                               "greedy randomized constructive heuristic"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search"));
    params.push_back(make_pair("OptFrame:float", "alpha parameter [0,1]"));
    params.push_back(make_pair("OptFrame:int", "max number of iterations"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicGRASPBuilder<S, XEv>::idComponent();
  }

  static std::string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":"
       << GRASP::family() << ":BasicGRASP";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_GRASP_BASICGRASP_HPP_
