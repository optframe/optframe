// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_BASICITERATEDLOCALSEARCH_HPP_
#define OPTFRAME_HEURISTICS_ILS_BASICITERATEDLOCALSEARCH_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <math.h>

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Search/SingleObjSearchBuilder.hpp>

#include "BasicILSPerturbation.hpp"
#include "FamilyILS.hpp"
#include "MetaILS.hpp"

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

MOD_EXPORT using BasicHistory = int;

MOD_EXPORT template <XESolution XES>
class BasicIteratedLocalSearch : public MetaILS<BasicHistory, XES> {
  using XEv = typename XES::second_type;

 protected:
  sref<LocalSearch<XES>> ls;
  sref<BasicILSPerturbation<XES>> p;
  int iterMax;

 public:
  BasicIteratedLocalSearch(sref<GeneralEvaluator<XES>> e,
                           sref<InitialSearch<XES>> constructive,
                           sref<LocalSearch<XES>> _ls,
                           sref<BasicILSPerturbation<XES>> _p, int _iterMax)
      : MetaILS<BasicHistory, XES>(e, constructive),
        ls(_ls),
        p(_p),
        iterMax(_iterMax) {}

  virtual ~BasicIteratedLocalSearch() {}

  uptr<BasicHistory> initializeHistory() override {
    uptr<int> iter(new int{});
    iter = 0;

    return iter;
  }

  void localSearch(XES& se, const StopCriteria<XEv>& sosc) override {
    ls->searchFrom(se, sosc);
  }

  void perturbation(XES& se, const StopCriteria<XEv>& sosc,
                    BasicHistory& history) override {
    int iter = history;

    p->perturb(se, sosc);

    // Incrementa a iteracao
    iter++;

    // Atualiza o historico
    history = iter;
  }

  bool acceptanceCriterion(const Evaluation<>& e1, const Evaluation<>& e2,
                           BasicHistory& history) override {
    if (MetaILS<BasicHistory, XES>::evaluator->betterStrict(e1, e2)) {
      // =======================
      //   Melhor solucao: 's2'
      // =======================
      std::cout << "Best fo: " << e1.evaluation();
      std::cout << " on [iter " << history << "]" << std::endl;

      // =======================
      //  Atualiza o historico
      // =======================
      // iter = 0
      history = 0;

      // =======================
      //    Retorna s2
      // =======================
      return true;
    } else {
      return false;
    }
  }

  bool terminationCondition(const BasicHistory& history) override {
    int iter = history;

    return (iter >= iterMax);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << MetaILS<BasicHistory, XES>::idComponent() << "BasicILS";
    return ss.str();
  }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class BasicIteratedLocalSearchBuilder : public FamilyILS,
                                        public SingleObjSearchBuilder<XES> {
 public:
  ~BasicIteratedLocalSearchBuilder() override = default;

  SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                              std::string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
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

    sptr<BasicILSPerturbation<XES>> pert;
    std::string comp_id3 = scanner.next();
    int id3 = *scanner.nextInt();
    hf.assign(pert, id3, comp_id3);

    int iterMax = *scanner.nextInt();

    // NOLINTNEXTLINE
    return new BasicIteratedLocalSearch<XES>(eval, constructive, h, pert,
                                             iterMax);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search"));
    params.push_back(std::make_pair(BasicILSPerturbation<XES>::idComponent(),
                                    "ils perturbation"));
    params.push_back(std::make_pair(
        "OptFrame:int", "max number of iterations without improvement"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicIteratedLocalSearch<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearchBuilder<XES>::idComponent() << ":"
       << FamilyILS::family() << "BasicILS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_BASICITERATEDLOCALSEARCH_HPP_
