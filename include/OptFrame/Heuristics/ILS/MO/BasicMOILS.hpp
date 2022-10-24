// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_BasicMOILS_HPP_
#define OPTFRAME_BasicMOILS_HPP_

#include <algorithm>

#include "BasicMOILSPerturbation.hpp"
#include "MultiObjILS.hpp"

namespace optframe {

typedef int BasicHistory;

template <XESolution XMES, XEvaluation XMEv = MultiEvaluation<>>
//template<XESolution XMES, XEvaluation XMEv=MultiEvaluation<>>
class BasicMOILS : public MultiObjILS<BasicHistory, typename XMES::first_type, XMEv, XMES> {
  using S = typename XMES::first_type;
  static_assert(is_same<S, typename XMES::first_type>::value);
  static_assert(is_same<XMEv, typename XMES::second_type>::value);
  using XEv = Evaluation<>;  // hardcoded.. TODO: fix
 private:
  sref<BasicMOILSPerturbation<XMES, XMEv>> p;
  int iterMax;

 public:
  BasicMOILS(sref<MultiEvaluator<XMES>> _mev, sref<InitialPareto<XMES>> _init_pareto, int _init_pop_size, sref<MOLocalSearch<XMES, XMEv>> _ls, sref<RandGen> _rg, sref<BasicMOILSPerturbation<XMES, XMEv>> _p, int _iterMax)
      :  //BasicMOILS(GeneralEvaluator<XMES, XMEv>& _mev, InitialPareto<XMES>& _init_pareto, int _init_pop_size, MOLocalSearch<S, XMEv>* _ls, RandGen& _rg, BasicMOILSPerturbation<XMES, XMEv>& _p, int _iterMax) :
        MultiObjILS<BasicHistory, S, XMEv, XMES>(_mev, _init_pareto, _init_pop_size, _ls, _rg),
        p(_p),
        iterMax(_iterMax) {
  }

  virtual ~BasicMOILS() {
  }

  virtual BasicHistory& initializeHistory() override {
    int& iter = *new int;
    iter = 0;

    return iter;
  }

  virtual void perturbation(XMES& smev, const StopCriteria<XMEv>& stopCriteria, BasicHistory& history) override {
    int iter = history;

    p->perturb(smev, stopCriteria);

    // Incrementa a iteracao
    iter++;

    // Atualiza o historico
    history = iter;
  }

  virtual void acceptanceCriterion(const Pareto<XMES>& pf, BasicHistory& history) override {
    if (pf.getNewNonDominatedSolutionsStatus()) {
      cout << "New Pareto size: is " << pf.size();
      cout << " on [iter without improvement " << history << "]" << endl;

      // =======================
      //  Atualiza o historico
      // =======================
      history = 0;
    }
  }

  virtual bool terminationCondition(BasicHistory& history) override {
    int iter = history;

    return (iter >= iterMax);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (MultiObjSearch<XMES>::compatible(s));
  }

  std::string id() const override {
    return idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << MultiObjILS<BasicHistory, S, XMEv>::idComponent() << "BasicMOILS";
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_BasicMOILS_HPP_*/
