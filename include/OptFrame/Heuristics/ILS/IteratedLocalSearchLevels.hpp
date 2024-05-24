// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCHLEVELS_HPP_
#define OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCHLEVELS_HPP_

#include <math.h>
//
#include <string>
#include <vector>
//
#include <OptFrame/LocalSearch.hpp>

#include "./ILS.h"
#include "./ILSLPerturbation.hpp"
#include "./IteratedLocalSearch.hpp"

namespace optframe {

typedef pair<pair<int, int>, pair<int, int>> levelHistory;

template <XESolution XES, XEvaluation XEv = typename XES::second_type>
class IteratedLocalSearchLevels
    : public IteratedLocalSearch<levelHistory, XES, XEv> {
 protected:
  sref<LocalSearch<XES>> ls;
  sref<ILSLPerturbation<XES, XEv>> p;
  int iterMax, levelMax;

 public:
  IteratedLocalSearchLevels(sref<GeneralEvaluator<XES>> e,
                            sref<InitialSearch<XES>> constructive,
                            sref<LocalSearch<XES>> _ls,
                            sref<ILSLPerturbation<XES, XEv>> _p, int _iterMax,
                            int _levelMax)
      : IteratedLocalSearch<levelHistory, XES, XEv>(e, constructive),
        ls(_ls),
        p(_p),
        iterMax(_iterMax),
        levelMax(_levelMax) {}

  virtual ~IteratedLocalSearchLevels() {}

  sref<levelHistory> initializeHistory() override {
    // cout << "initializeHistory()" << endl;
    pair<int, int> vars(0, 0);

    // IterMax e LevelMax
    pair<int, int> maxs(iterMax, levelMax);

    return sref<levelHistory>(new levelHistory(vars, maxs));
  }

  void localSearch(XES& se, const StopCriteria<XEv>& stopCriteria) override {
    // cout << "localSearch(.)" << endl;
    ls->searchFrom(se, stopCriteria);
  }

  void perturbation(XES& se, const StopCriteria<XEv>& stopCriteria,
                    sref<levelHistory> history) override {
    // cout << "perturbation(.)" << endl;

    int iter = history->first.first;
    int level = history->first.second;
    int iterMax = history->second.first;
    // int levelMax = history.second.second;

    if (Component::debug)
      cout << "ILSL::perturbation() history iter " << iter << " level " << level
           << endl;

    // cout << "level = " << level << " e iter = " << iter << endl;

    // nivel atual: 'level'
    p->perturb(se, stopCriteria, level);

    // Incrementa a iteracao
    iter++;

    if (Component::debug) cout << "ILSL::perturbation() iter " << iter << endl;

    if (iter >= iterMax) {
      iter = 0;
      level++;
      if (Component::information)
        cout << "ILSL::perturbation() level " << level << ".." << endl;
    }

    // Atualiza o historico
    history->first.first = iter;
    history->first.second = level;
    if (Component::debug)
      cout << "ILSL::new history iter " << iter << " level " << level << endl;
  }

  bool acceptanceCriterion(const XEv& e1, const XEv& e2,
                           sref<levelHistory> history) override {
    // cout << "acceptanceCriterion(.)" << endl;

    // if (IteratedLocalSearch<levelHistory, XES, XEv>::evaluator.betterThan(e1,
    // e2)) if (e1.betterStrict(e2))
    if (Component::debug)
      std::cout << "ILSL::acceptanceCriterion() will compare(outdated="
                << e1.isOutdated() << " e=" << e1.evaluation()
                << "; outdated=" << e2.isOutdated() << " e=" << e2.evaluation()
                << ")" << std::endl;

    if (IteratedLocalSearch<levelHistory, XES, XEv>::evaluator->betterStrict(
            e1, e2)) {
      if (Component::information) {
        cout << "ILSL::acceptanceCriterion() Best fo: on [iter "
             << history->first.first << " of level " << history->first.second
             << "] => ";
        e1.print();
      }

      // =======================
      //  Atualiza o historico
      // =======================
      // iter = 0
      history->first.first = 0;
      // level = 0
      history->first.second = 0;

      // =======================
      //    Retorna s2
      // =======================
      return true;
    } else {
      return false;
    }
  }

  bool terminationCondition(sref<levelHistory> history) override {
    // cout << "terminationCondition(.)" << endl;
    int level = history->first.second;
    int levelMax = history->second.second;

    return (level >= levelMax);
  }

  std::string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << IteratedLocalSearch<levelHistory, XES, XEv>::idComponent() << "ILSL";
    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class IteratedLocalSearchLevelsBuilder : public ILS,
                                         public SingleObjSearchBuilder<XES> {
 public:
  ~IteratedLocalSearchLevelsBuilder() override = default;

  SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                              string family = "") override {
    sptr<GeneralEvaluator<XES>> eval = nullptr;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(eval, id_0, sid_0);
    if (!eval) return nullptr;

    // Constructive<S>* constructive = nullptr;
    sptr<InitialSearch<XES>> constructive = nullptr;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(constructive, id_1, sid_1);
    if (!constructive) return nullptr;

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES>> h = method.first;

    scanner = Scanner(method.second);
    if (!h) return nullptr;

    sptr<ILSLPerturbation<XES>> pert;
    std::string sid_3 = scanner.next();
    int id_3 = *scanner.nextInt();
    hf.assign(pert, id_3, sid_3);
    if (!pert) return nullptr;

    int iterMax = -1;

    auto oiterMax = scanner.nextInt();

    if (!oiterMax) {
      return nullptr;
    }

    iterMax = *oiterMax;

    int levelMax = -1;

    auto olevelMax = scanner.nextInt();

    if (!olevelMax) {
      return nullptr;
    }

    levelMax = *olevelMax;

    return new IteratedLocalSearchLevels<XES>(eval, constructive, h, pert,
                                              iterMax, levelMax);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    // params.push_back(make_pair(Constructive<S>::idComponent(), "constructive
    // heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search"));
    params.push_back(
        make_pair(ILSLPerturbation<XES>::idComponent(), "ilsL perturbation"));
    params.push_back(
        make_pair("int", "max number of iterations without improvement"));
    params.push_back(make_pair("int", "levelMax of perturbation"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == IteratedLocalSearchLevels<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<XES>::idComponent() << ":" << ILS::family()
       << "ILSLevels";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_ITERATEDLOCALSEARCHLEVELS_HPP_
