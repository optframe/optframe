// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_BASICILSPERTURBATION_HPP_
#define OPTFRAME_HEURISTICS_ILS_BASICILSPERTURBATION_HPP_

#include <math.h>

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/NS.hpp>
#include <OptFrame/RandGen.hpp>

#include "ILS.h"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicILSPerturbation : public ILS, public Component {
 private:
  sref<GeneralEvaluator<XES, XEv>> evaluator;
  int pMin;
  int pMax;
  vsref<NS<XES, XEv>> ns;
  sref<RandGen> rg;

 public:
  BasicILSPerturbation(sref<GeneralEvaluator<XES, XEv>> e, int _pMin, int _pMax,
                       vsref<NS<XES, XEv>>& _ns, sref<RandGen> _rg)
      : evaluator(e), pMin(_pMin), pMax(_pMax), ns(_ns), rg(_rg) {
    if (pMax < pMin) {
      cout << "BasicILSPerturbation warning: pMax > pMin! Swapping both."
           << endl;
      int aux = pMax;
      pMax = pMin;
      pMin = aux;
    }

    if (ns.size() == 0)
      cout << "BasicILSPerturbation warning: empty neighborhood list." << endl;
  }

  BasicILSPerturbation(sref<GeneralEvaluator<XES, XEv>> e, int _pMin, int _pMax,
                       sref<NS<XES, XEv>> _ns, sref<RandGen> _rg)
      : evaluator(e), pMin(_pMin), pMax(_pMax), rg(_rg) {
    ns.push_back(&_ns);
    if (pMax < pMin) {
      cout << "BasicILSPerturbation warning: pMax > pMin! Swapping both."
           << endl;
      int aux = pMax;
      pMax = pMin;
      pMin = aux;
    }

    if (ns.size() == 0)
      cout << "BasicILSPerturbation warning: empty neighborhood list." << endl;
  }

  virtual ~BasicILSPerturbation() {}

  void add_ns(NS<XES, XEv>& _ns) { ns.push_back(&_ns); }

  void perturb(XES& se,
               const StopCriteria<XEv>& stopCriteria)  // TODO: override?? what?
  {
    // XSolution& s = se.first;
    // XEv& e = se.second;
    //
    for (int i = pMin; i < pMax; i++) {
      int nk = rand() % ns.size();

      uptr<Move<XES, XEv>> mp = ns[nk]->validRandomMove(se);

      if (!mp) {
        cout << "BasicILSPerturbation warning: perturbation found no valid "
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

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":" << ILS::family() << "basic_pert";
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicILSPerturbationBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~BasicILSPerturbationBuilder() {}

  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
    sptr<GeneralEvaluator<XES, XEv>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    int pMin = *scanner.nextInt();
    int pMax = *scanner.nextInt();

    vsptr<NS<XES, XEv>> _ns_list;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assignList(_ns_list, id2, comp_id2);

    vsref<NS<XES, XEv>> ns_list;
    for (auto x : _ns_list) ns_list.push_back(x);

    return new BasicILSPerturbation<XES, XEv>(eval, pMin, pMax, ns_list,
                                              hf.getRandGen());
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(),
                               "evaluation function"));
    params.push_back(make_pair("OptFrame:int", "pMin: min number of moves"));
    params.push_back(make_pair("OptFrame:int", "pMax: max number of moves"));
    stringstream ss;
    ss << NS<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of neighborhood structures"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicILSPerturbation<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << ILS::family()
       << "basic_pert";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_BASICILSPERTURBATION_HPP_
