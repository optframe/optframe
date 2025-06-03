// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_BASICILSPERTURBATION_HPP_
#define OPTFRAME_HEURISTICS_ILS_BASICILSPERTURBATION_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <math.h>

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>

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

MOD_EXPORT template <XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicILSPerturbation : public ILS, public Component {
  using XSH = XES;  // primary-based search type only (BestType)

 private:
  sref<GeneralEvaluator<XES>> evaluator;
  int pMin;
  int pMax;
  vsref<NS<XES, XSH>> ns;
  sref<RandGen> rg;

 public:
  BasicILSPerturbation(sref<GeneralEvaluator<XES>> e, int _pMin, int _pMax,
                       vsref<NS<XES, XSH>>& _ns, sref<RandGen> _rg)
      : evaluator(e), pMin(_pMin), pMax(_pMax), ns(_ns), rg(_rg) {
    if (pMax < pMin) {
      std::cout << "BasicILSPerturbation warning: pMax > pMin! Swapping both."
                << std::endl;
      int aux = pMax;
      pMax = pMin;
      pMin = aux;
    }

    if (ns.size() == 0)
      std::cout << "BasicILSPerturbation warning: empty neighborhood list."
                << std::endl;
  }

  BasicILSPerturbation(sref<GeneralEvaluator<XES>> e, int _pMin, int _pMax,
                       sref<NS<XES, XSH>> _ns, sref<RandGen> _rg)
      : evaluator(e), pMin(_pMin), pMax(_pMax), rg(_rg) {
    ns.push_back(&_ns);
    if (pMax < pMin) {
      std::cout << "BasicILSPerturbation warning: pMax > pMin! Swapping both."
                << std::endl;
      int aux = pMax;
      pMax = pMin;
      pMin = aux;
    }

    if (ns.size() == 0)
      std::cout << "BasicILSPerturbation warning: empty neighborhood list."
                << std::endl;
  }

  virtual ~BasicILSPerturbation() {}

  void add_ns(NS<XES, XSH>& _ns) { ns.push_back(&_ns); }

  void perturb(XES& se,
               const StopCriteria<XEv>& stopCriteria)  // TODO: override?? what?
  {
    // XSolution& s = se.first;
    // XEv& e = se.second;
    //
    for (int i = pMin; i < pMax; i++) {
      int nk = rg->rand() % ns.size();

      uptr<Move<XES, XSH>> mp = ns[nk]->validRandomMove(se);

      if (!mp) {
        std::cout
            << "BasicILSPerturbation warning: perturbation found no valid "
               "move for neighborhood: ";
        ns[nk]->print();
      } else {
        mp->applyUpdate(se);
      }
    }

    evaluator->reevaluate(se);  // updates 'e'
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << ILS::family() << "basic_pert";
    return ss.str();
  }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class BasicILSPerturbationBuilder : public ComponentBuilder<XES> {
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~BasicILSPerturbationBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    int pMin = *scanner.nextInt();
    int pMax = *scanner.nextInt();

    vsptr<NS<XES, XSH>> _ns_list;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assignList(_ns_list, id2, comp_id2);

    vsref<NS<XES, XSH>> ns_list;
    for (auto x : _ns_list) ns_list.push_back(x);

    return new BasicILSPerturbation<XES>(eval, pMin, pMax, ns_list,
                                         hf.getRandGen());
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        std::make_pair("OptFrame:int", "pMin: min number of moves"));
    params.push_back(
        std::make_pair("OptFrame:int", "pMax: max number of moves"));
    std::stringstream ss;
    ss << NS<XES, XSH>::idComponent() << "[]";
    params.push_back(
        std::make_pair(ss.str(), "list of neighborhood structures"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicILSPerturbation<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << ILS::family() << "basic_pert";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_BASICILSPERTURBATION_HPP_
