// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_ILSLPERTURBATION_HPP_
#define OPTFRAME_HEURISTICS_ILS_ILSLPERTURBATION_HPP_

#include <math.h>

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/StopCriteria.hpp>

#include "ILS.h"

namespace optframe {

template <XESolution XES, XEvaluation XEv = typename XES::second_type>
class ILSLPerturbation : public Component, public ILS {
 public:
  ~ILSLPerturbation() override = default;

  virtual void perturb(XES& se, const StopCriteria<XEv>& stopCriteria,
                       int level) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":" << ILS::family() << "LevelPert";
    return ss.str();
  }
};

template <XESolution XES, XEvaluation XEv = typename XES::second_type>
class ILSLPerturbationLPlus2 : public ILSLPerturbation<XES, XEv> {
  using XSH = XES;  // primary-based search type only (BestType)

 private:
  vsref<NS<XES, XSH>> ns;
  sref<GeneralEvaluator<XES>> evaluator;
  sref<RandGen> rg;

 public:
  ILSLPerturbationLPlus2(sref<GeneralEvaluator<XES>> e, sref<NS<XES, XSH>> _ns,
                         sref<RandGen> _rg)
      : evaluator(e), rg(_rg) {
    ns.push_back(_ns);
  }

  virtual ~ILSLPerturbationLPlus2() = default;

  void add_ns(sref<NS<XES, XSH>> _ns) { ns.push_back(_ns); }

  void perturb(XES& se, const StopCriteria<XEv>& stopCriteria,
               int level) override {
    int a = 0;  // number of appliable moves

    level += 2;  // level 0 applies 2 moves

    // local bind
    // XSolution& s = se.first;
    // XEv& e = se.second;

    while (a < level) {
      int x = rg->rand(ns.size());

      uptr<Move<XES, XSH>> m = ns[x]->validRandomMove(se);

      if (m) {
        a++;
        m->applyUpdate(se);
      } else {
        if (Component::warning)
          cout << "ILS Warning: perturbation had no effect in level " << a
               << "!" << endl;
      }
    }

    evaluator->reevaluate(se);  // updates 'e'
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (ILSLPerturbation<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << ILSLPerturbation<XES, XEv>::idComponent() << ":LPlus2";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

template <XESolution XES, XEvaluation XEv = typename XES::second_type>
class ILSLPerturbationLPlus2Prob : public ILSLPerturbation<XES, XEv> {
  using XSH = XES;  // primary-based search type only (BestType)

 private:
  vsref<NS<XES, XSH>> ns;
  vector<pair<int, double>> pNS;
  sref<GeneralEvaluator<XES>> evaluator;
  sref<RandGen> rg;

 public:
  ILSLPerturbationLPlus2Prob(sref<GeneralEvaluator<XES>> e,
                             sref<NS<XES, XSH>> _ns, sref<RandGen> _rg)
      : evaluator(e), rg(_rg) {
    ns.push_back(_ns);
    pNS.push_back(make_pair(1, 1));
  }

  virtual ~ILSLPerturbationLPlus2Prob() = default;

  void add_ns(sref<NS<XES, XSH>> _ns) {
    ns.push_back(_ns);
    pNS.push_back(make_pair(1, 1));

    double soma = 0;
    for (int i = 0; i < ns.size(); i++) soma += pNS[i].first;
    for (int i = 0; i < ns.size(); i++) pNS[i].second = pNS[i].first / soma;
  }

  void changeProb(vector<int> pri) {
    int nNeighborhoods = ns.size();
    if (pri.size() != nNeighborhoods) {
      cout << "ERROR ON PRIORITES SIZE!" << endl;
      return;
    }

    double soma = 0;
    for (int i = 0; i < nNeighborhoods; i++) {
      pNS[i].first = pri[i];
      soma += pri[i];
    }

    for (int i = 0; i < ns.size(); i++) pNS[i].second = pNS[i].first / soma;

    cout << "Printing probabilities ILSLPerturbationLPlus2Prob:" << endl;
    for (int i = 0; i < ns.size(); i++)
      cout << "pNS[i].first: " << pNS[i].first
           << "\t pNS[i].second: " << pNS[i].second << endl;
    cout << endl;
  }

  void perturb(XES& se, const StopCriteria<XEv>& stopCriteria,
               int level) override {
    // XSolution& s = se.first;
    // XEv& e = se.second;
    //
    int a = 0;  // number of appliable moves

    level += 2;  // level 0 applies 2 moves

    while (a < level) {
      double prob = rg->rand01();
      int x = 0;
      double sum = pNS[x].second;

      while (prob > sum) {
        x++;
        sum += pNS[x].second;
      }

      uptr<Move<XES, XSH>> m = ns[x]->validRandomMove(se);

      if (m) {
        a++;
        m->applyUpdate(se);
      } else {
        if (Component::warning)
          cout << "ILS Warning: perturbation had no effect in level " << a
               << "!" << endl;
      }
    }

    evaluator->reevaluate(se);  // updates 'e'
  }

  static string idComponent() {
    stringstream ss;
    ss << ILSLPerturbation<XES, XEv>::idComponent() << ":LPlus2Prob";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class ILSLPerturbationLPlus2Builder : public ComponentBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~ILSLPerturbationLPlus2Builder() = default;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(eval, id_0, sid_0);

    sptr<NS<XES, XSH>> ns;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(ns, id_1, sid_1);

    // NOLINTNEXTLINE
    return new ILSLPerturbationLPlus2<XES>(eval, ns, hf.getRandGen());
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<S, XEv, XES>::idComponent(),
                               "evaluation function"));
    params.push_back(
        make_pair(NS<XES, XSH>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == ILSLPerturbationLPlus2<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << ILS::family()
       << "LevelPert:LPlus2";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class ILSLPerturbationLPlus2ProbBuilder : public ComponentBuilder<XES> {
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~ILSLPerturbationLPlus2ProbBuilder() = default;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<NS<XES, XSH>> ns;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(ns, id2, comp_id2);

    // NOLINTNEXTLINE
    return new ILSLPerturbationLPlus2Prob<XES>(eval, ns, hf.getRandGen());
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NS<XES, XSH>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == ILSLPerturbationLPlus2Prob<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << ILS::family()
       << "LevelPert:LPlus2Prob";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_ILSLPERTURBATION_HPP_
