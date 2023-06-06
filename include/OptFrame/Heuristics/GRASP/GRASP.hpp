// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_GRASP_GRASP_HPP_
#define OPTFRAME_HEURISTICS_GRASP_GRASP_HPP_

#include <string>
#include <utility>
#include <vector>

#include "../../LocalSearch.hpp"
#include "../../SingleObjSearch.hpp"
#include "GRASP.h"
#include "GRConstructive.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class GRASP : public SingleObjSearch<XES> {
 private:
  Evaluator<S>& evaluator;
  InitialSearch<XES>& constructive;
  LocalSearch<XES, XEv>& ls;
  unsigned int iterMax;

 public:
  GRASP(Evaluator& _eval, InitialSearch<XES>& _constructive,
        LocalSearch<XES, XEv>& _ls, int _iterMax)
      : evaluator(_eval), constructive(_constructive), ls(_ls) {
    iterMax = _iterMax;
  }

  virtual ~GRASP() {}

  pair<Solution<R, ADS>&, Evaluation<DS>&>* search(
      double timelimit = 100000000, double target_f = 0,
      const Solution<R, ADS>* _s = nullptr,
      const Evaluation<DS>* _e = nullptr) {
    long tini = time(nullptr);

    unsigned int iter = 0;

    long tnow = time(nullptr);

    Solution<R, ADS>& s = constructive.generateSolution();
    Evaluation<>& e = evaluator.evaluate(s);

    while ((iter < iterMax) && ((tnow - tini) < timelimit) &&
           (evaluator.betterThan(target_f, e.evaluation()))) {
      if (Component::verbose) cout << "GRASP::iter=" << iter << endl;

      Solution<R, ADS>& s1 = constructive.generateSolution();
      Evaluation<DS>& e1 = evaluator.evaluate(s1);

      ls.searchFrom(s1, e1, timelimit, target_f);

      if (evaluator.betterThan(e1, e)) {
        s = s1;
        e = e1;
        if (Component::information) {
          cout << "GRASP iter " << iter << ": ";
          e.print();
        }
      }

      delete &s1;
      delete &e1;

      tnow = time(nullptr);
      iter++;
    }

    return new pair<Solution<R, ADS>&, Evaluation<DS>&>(s, e);
  }

  std::string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<R, ADS, DS>::idComponent() << GRASPH::family()
       << "GRASP";
    return ss.str();
  }
};

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class GRASPBuilder : public GRASPH, public SingleObjSearchBuilder<R, ADS, DS> {
 public:
  virtual ~GRASPBuilder() {}

  virtual SingleObjSearch<R, ADS, DS>* build(Scanner& scanner,
                                             HeuristicFactory<R, ADS, DS>& hf,
                                             string family = "") {
    Evaluator<R, ADS, DS>* eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    Constructive<S>* constructive;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(constructive, id2, comp_id2);

    string rest = scanner.rest();

    pair<LocalSearch<R, ADS, DS>*, std::string> method;
    method = hf.createLocalSearch(rest);

    LocalSearch<R, ADS, DS>* h = method.first;

    scanner = Scanner(method.second);

    if (!scanner.hasNext()) return nullptr;

    int iterMax = *scanner.nextInt();

    return new GRASP<R, ADS, DS>(*eval, *constructive, *h, iterMax);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
    // params.push_back(make_pair(Constructive<S>::idComponent(), "constructive
    // heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(
        make_pair(LocalSearch<R, ADS, DS>::idComponent(), "local search"));
    params.push_back(make_pair("int", "max number of iterations"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == GRASPBuilder<R, ADS, DS>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<R, ADS, DS>::idComponent() << GRASPH::family()
       << "GRASP";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_GRASP_GRASP_HPP_
