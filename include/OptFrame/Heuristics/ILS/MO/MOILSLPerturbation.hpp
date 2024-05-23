// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MOILSLPerturbation_HPP_
#define OPTFRAME_MOILSLPerturbation_HPP_

#include <math.h>

#include <vector>

#include "../../../MultiObjSearch.hpp"
#include "../../../NS.hpp"
#include "../../../RandGen.hpp"
#include "MOILS.h"

namespace optframe {

// template<XSolution S, XEvaluation XEv=Evaluation<>>
template <XESolution XMES, XEvaluation XMEv = MultiEvaluation<>>
class MOILSLPerturbation : public Component, public MOILS {
 public:
  virtual ~MOILSLPerturbation() {}

  virtual void perturb(XMES& smev, const StopCriteria<XMEv>& stopCriteria,
                       int level) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << MOILS::family() << "LevelPert";
    return ss.str();
  }
};

// template<XSolution S, XEvaluation XEv=Evaluation<>, XEvaluation
// XMEv=MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
template <XESolution XMES, XEvaluation XMEv = MultiEvaluation<>>
class MOILSLPerturbationLPlus2 : public MOILSLPerturbation<XMES, XMEv> {
 private:
  vsref<NS<XMES>> ns;
  sref<GeneralEvaluator<XMES>> evaluator;
  sref<RandGen> rg;

 public:
  MOILSLPerturbationLPlus2(sref<GeneralEvaluator<XMES>> _e, sref<NS<XMES>> _ns,
                           sref<RandGen> _rg)
      : evaluator(_e), rg(_rg) {
    ns.push_back(_ns);
  }

  virtual ~MOILSLPerturbationLPlus2() {}

  void add_ns(sref<NS<XMES>> _ns) { ns.push_back(_ns); }

  // void perturb(S& s, XMEv& mev, const StopCriteria<XEv>& stopCriteria, int
  // level)
  void perturb(XMES& smev, const StopCriteria<XMEv>& stopCriteria,
               int level) override {
    // XES se = make_pair(s, Evaluation<>()); // TODO: multiev

    int a = 0;  // number of appliable moves

    level += 2;  // level 0 applies 2 moves

    while (a < level) {
      int x = rg->rand(ns.size());

      uptr<Move<XMES>> m = ns[x]->validRandomMove(smev);

      if (m) {
        a++;
        // Component::safe_delete(m->applyMEVUpdate(mev, s));
        // m->applyMEVUpdate(mev, se);
        m->applyUpdate(smev);
      } else if (Component::warning)
        cout << "ILS Warning: perturbation had no effect in level " << a << "!"
             << endl;

      // delete m;
    }

    evaluator->reevaluate(smev);  // updates 'mev'
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) ||
           (MOILSLPerturbation<XMES, XMEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << MOILSLPerturbation<XMES, XMEv>::idComponent() << ":LPlus2";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// template<XSolution S, XEvaluation XEv=Evaluation<>, XESolution XES = pair<S,
// XEv>>
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XEvaluation XMEv = MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
class MOILSLPerturbationLPlus2Prob : public MOILSLPerturbation<XMES, XMEv> {
 private:
  vector<NS<XMES>*> ns;
  vector<pair<int, double>> pNS;
  IEvaluator<XMES>& evaluator;
  RandGen& rg;

 public:
  MOILSLPerturbationLPlus2Prob(IEvaluator<XMES>& _e, NS<XMES>& _ns,
                               RandGen& _rg)
      : evaluator(_e), rg(_rg) {
    ns.push_back(&_ns);
    pNS.push_back(make_pair(1, 1));
  }

  virtual ~MOILSLPerturbationLPlus2Prob() {}

  void add_ns(NS<XMES>& _ns) {
    ns.push_back(&_ns);
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

  // void perturb(S& s, MultiEvaluation<>& mev, const StopCriteria<XEv>&
  // stopCriteria, int level) override
  void perturb(XMES& smev, const StopCriteria<XMEv>& stopCriteria,
               int level) override {
    int a = 0;  // number of appliable moves

    level += 2;  // level 0 applies 2 moves

    while (a < level) {
      double prob = rg.rand01();
      int x = 0;
      double sum = pNS[x].second;

      while (prob > sum) {
        x++;
        sum += pNS[x].second;
      }

      Move<S, XEv>* m = ns[x]->validMove(smev);

      if (m) {
        a++;
        // Component::safe_delete(m->applyMEVUpdateSolution(mev, s));
        m->applyUpdate(smev);
      } else if (Component::warning)
        cout << "ILS Warning: perturbation had no effect in level " << a << "!"
             << endl;

      delete m;
    }

    // evaluator.reevaluateMEV(mev, s); // updates 'e'
    evaluator.reevaluate(smev);  // updates 'mev'
  }

  static string idComponent() {
    stringstream ss;
    ss << MOILSLPerturbation<XMES, XMEv>::idComponent() << ":LPlus2Prob";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_MOILSLPerturbation_HPP_*/
