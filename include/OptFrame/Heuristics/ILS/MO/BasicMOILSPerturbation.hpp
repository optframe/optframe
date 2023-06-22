// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_MOBASICILSPerturbation_HPP_
#define OPTFRAME_MOBASICILSPerturbation_HPP_

#include <math.h>

#include <vector>

#include "../../../ComponentBuilder.h"
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
  sref<GeneralEvaluator<XMES, XMEv>> mEval;
  int pMin;
  int pMax;
  vsref<NS<XMES, XMEv>> ns;
  sref<RandGen> rg;

 public:
  // BasicMOILSPerturbation(MultiEvaluator<S, XEv>& _mEval, int _pMin, int
  // _pMax, vector<NS<XES, XSH>*>& _ns, RandGen& _rg) :
  BasicMOILSPerturbation(sref<GeneralEvaluator<XMES, XMEv>> _mEval, int _pMin,
                         int _pMax, vsref<NS<XMES, XMEv>> _ns,
                         sref<RandGen> _rg)
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
  BasicMOILSPerturbation(sref<GeneralEvaluator<XMES, XMEv>> _mEval, int _pMin,
                         int _pMax, sref<NS<XMES, XMEv>> _ns, sref<RandGen> _rg)
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

  virtual ~BasicMOILSPerturbation() {}

  void add_ns(sref<NS<XMES, XMEv>> _ns) { ns.push_back(_ns); }

  // void perturb(S& s, MultiEvaluation<>& mev, const StopCriteria<XEv>&
  // stopCriteria)
  void perturb(XMES& smev, const StopCriteria<XMEv>& stopCriteria) {
    // XES se = make_pair(s, Evaluation<>());
    //
    for (int i = pMin; i < pMax; i++) {
      int nk = rand() % ns.size();

      uptr<Move<XMES, XMEv>> mp = ns[nk]->validRandomMove(smev);

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

  virtual string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":" << ILS::family() << "MObasic_pert";
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MOBASICILSPerturbation_HPP_*/
