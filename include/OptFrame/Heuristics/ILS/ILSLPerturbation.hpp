// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_ILSLPERTURBATION_HPP_
#define OPTFRAME_HEURISTICS_ILS_ILSLPERTURBATION_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <math.h>

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Search/StopCriteria.hpp>

#include "ILS.hpp"

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

MOD_EXPORT template <XESolution XES,
                     XEvaluation XEv = typename XES::second_type>
class ILSLPerturbation : public Component, public ILS {
 public:
  ~ILSLPerturbation() override = default;

  virtual void perturb(XES& se, const StopCriteria<XEv>& stopCriteria,
                       int level) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << ILS::family() << "LevelPert";
    return ss.str();
  }
};

MOD_EXPORT template <XESolution XES,
                     XEvaluation XEv = typename XES::second_type>
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
          std::cout << "ILS Warning: perturbation had no effect in level " << a
                    << "!" << std::endl;
      }
    }

    evaluator->reevaluate(se);  // updates 'e'
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (ILSLPerturbation<XES, XEv>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ILSLPerturbation<XES, XEv>::idComponent() << ":LPlus2";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

MOD_EXPORT template <XESolution XES,
                     XEvaluation XEv = typename XES::second_type>
class ILSLPerturbationLPlus2Prob : public ILSLPerturbation<XES, XEv> {
  using XSH = XES;  // primary-based search type only (BestType)

 private:
  vsref<NS<XES, XSH>> ns;
  std::vector<std::pair<int, double>> pNS;
  sref<GeneralEvaluator<XES>> evaluator;
  sref<RandGen> rg;

 public:
  ILSLPerturbationLPlus2Prob(sref<GeneralEvaluator<XES>> e,
                             sref<NS<XES, XSH>> _ns, sref<RandGen> _rg)
      : evaluator(e), rg(_rg) {
    ns.push_back(_ns);
    pNS.push_back(std::make_pair(1, 1));
  }

  virtual ~ILSLPerturbationLPlus2Prob() = default;

  void add_ns(sref<NS<XES, XSH>> _ns) {
    ns.push_back(_ns);
    pNS.push_back(std::make_pair(1, 1));

    double soma = 0;
    for (int i = 0; i < ns.size(); i++) soma += pNS[i].first;
    for (int i = 0; i < ns.size(); i++) pNS[i].second = pNS[i].first / soma;
  }

  void changeProb(std::vector<int> pri) {
    int nNeighborhoods = ns.size();
    if (pri.size() != nNeighborhoods) {
      std::cout << "ERROR ON PRIORITES SIZE!" << std::endl;
      return;
    }

    double soma = 0;
    for (int i = 0; i < nNeighborhoods; i++) {
      pNS[i].first = pri[i];
      soma += pri[i];
    }

    for (int i = 0; i < ns.size(); i++) pNS[i].second = pNS[i].first / soma;

    std::cout << "Printing probabilities ILSLPerturbationLPlus2Prob:"
              << std::endl;
    for (int i = 0; i < ns.size(); i++)
      std::cout << "pNS[i].first: " << pNS[i].first
                << "\t pNS[i].second: " << pNS[i].second << std::endl;
    std::cout << std::endl;
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
          std::cout << "ILS Warning: perturbation had no effect in level " << a
                    << "!" << std::endl;
      }
    }

    evaluator->reevaluate(se);  // updates 'e'
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ILSLPerturbation<XES, XEv>::idComponent() << ":LPlus2Prob";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_ILSLPERTURBATION_HPP_
