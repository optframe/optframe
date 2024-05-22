// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MOBASICILSPerturbation_HPP_
#define OPTFRAME_MOBASICILSPerturbation_HPP_

#include <math.h>

#include <vector>

#include "../../../Hyper/ComponentBuilder.hpp"
#include "../../../MultiEvaluator.hpp"
#include "../../../MultiObjSearch.hpp"
#include "../../../NS.hpp"
#include "../../../RandGen.hpp"
#include "MOILS.h"

namespace optframe {

// template<XSolution S, XEvaluation XEv=Evaluation<>, XESolution XES = pair<S,
// XEv>>
template <XESolution XMES, XEvaluation XMEv = MultiEvaluation<>>
class BasicMOILSPerturbation : public MOILS, public Component {
 private:
  // MultiEvaluator<S, XEv>& mEval;
  sref<GeneralEvaluator<XMES>> mEval;
  int pMin;
  int pMax;
  vsref<NS<XMES>> ns;
  sref<RandGen> rg;

 public:
  // BasicMOILSPerturbation(MultiEvaluator<S, XEv>& _mEval, int _pMin, int
  // _pMax, vector<NS<XES, XSH>*>& _ns, RandGen& _rg) :
  BasicMOILSPerturbation(sref<GeneralEvaluator<XMES>> _mEval, int _pMin,
                         int _pMax, vsref<NS<XMES>> _ns, sref<RandGen> _rg)
      : mEval(_mEval), pMin(_pMin), pMax(_pMax), ns(_ns), rg(_rg) {
    if (pMax < pMin) {
      cout << "BasicMOILSPerturbation warning: pMax > pMin! Swapping both."
           << endl;
      int aux = pMax;
      pMax = pMin;
      pMin = aux;
    }

    if (ns.size() == 0)
      cout << "BasicMOILSPerturbation warning: empty neighborhood list."
           << endl;
  }

  // BasicMOILSPerturbation(MultiEvaluator<S, XEv>& _mEval, int _pMin, int
  // _pMax, NS<XES, XSH>& _ns, RandGen& _rg) :
  BasicMOILSPerturbation(sref<GeneralEvaluator<XMES>> _mEval, int _pMin,
                         int _pMax, sref<NS<XMES>> _ns, sref<RandGen> _rg)
      : mEval(_mEval), pMin(_pMin), pMax(_pMax), rg(_rg) {
    ns.push_back(_ns);
    if (pMax < pMin) {
      cout << "BasicMOILSPerturbation warning: pMax > pMin! Swapping both."
           << endl;
      int aux = pMax;
      pMax = pMin;
      pMin = aux;
    }

    if (ns.size() == 0)
      cout << "BasicMOILSPerturbation warning: empty neighborhood list."
           << endl;
  }

  ~BasicMOILSPerturbation() override {}

  void add_ns(sref<NS<XMES>> _ns) { ns.push_back(_ns); }

  // void perturb(S& s, MultiEvaluation<>& mev, const StopCriteria<XEv>&
  // stopCriteria)
  void perturb(XMES& smev, const StopCriteria<XMEv>& stopCriteria) {
    // XES se = make_pair(s, Evaluation<>());
    //
    for (int i = pMin; i < pMax; i++) {
      int nk = rand() % ns.size();

      uptr<Move<XMES>> mp = ns[nk]->validRandomMove(smev);

      if (!mp) {
        cout << "BasicMOILSPerturbation warning: perturbation found no valid "
                "move for neighborhood: ";
        ns[nk]->print();
      } else {
        // Move<XES, XSH>& m = *mp;
        // Component::safe_delete(m.applyMEVUpdate(mev, s));
        // delete &m;
        // mp->applyMEVUpdate(mev, se);
        mp->applyUpdate(smev);
      }
    }

    // mEval.reevaluateMEV(mev, se); // updates 'e'
    mEval->reevaluate(smev);  // updates 'e'
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":" << ILS::family() << "MObasic_pert";
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MOBASICILSPerturbation_HPP_*/
