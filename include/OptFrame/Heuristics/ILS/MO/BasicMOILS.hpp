// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_BasicMOILS_HPP_
#define OPTFRAME_BasicMOILS_HPP_

#include <algorithm>
#include <string>
//

#include "./BasicMOILSPerturbation.hpp"
#include "./MultiObjILS.hpp"

namespace optframe {

typedef int BasicHistory;

template <XESolution XES, XEMSolution XMES>
class BasicMOILS : public MultiObjILS<BasicHistory, typename XMES::first_type,
                                      typename XMES::second_type, XMES> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XES::second_type;

 private:
  sref<BasicMOILSPerturbation<XMES, XMEv>> p;
  int iterMax;

 public:
  BasicMOILS(sref<MultiEvaluator<XES, XMES>> _mev,
             sref<InitialPareto<XMES>> _init_pareto, int _init_pop_size,
             sref<MOLocalSearch<XES, XMES>> _ls, sref<RandGen> _rg,
             sref<BasicMOILSPerturbation<XMES, XMEv>> _p, int _iterMax)
      : MultiObjILS<BasicHistory, S, XMEv, XMES>(_mev, _init_pareto,
                                                 _init_pop_size, _ls, _rg),
        p(_p),
        iterMax(_iterMax) {}

  virtual ~BasicMOILS() {}

  BasicHistory& initializeHistory() override {
    int& iter = *new int;
    iter = 0;

    return iter;
  }

  void perturbation(XMES& smev, const StopCriteria<XMEv>& stopCriteria,
                    BasicHistory& history) override {
    int iter = history;

    p->perturb(smev, stopCriteria);

    // Incrementa a iteracao
    iter++;

    // Atualiza o historico
    history = iter;
  }

  void acceptanceCriterion(const Pareto<XMES>& pf,
                           BasicHistory& history) override {
    if (pf.getNewNonDominatedSolutionsStatus()) {
      cout << "New Pareto size: is " << pf.size();
      cout << " on [iter without improvement " << history << "]" << endl;

      // =======================
      //  Atualiza o historico
      // =======================
      history = 0;
    }
  }

  bool terminationCondition(BasicHistory& history) override {
    int iter = history;

    return (iter >= iterMax);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (MultiObjSearch<XMES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    stringstream ss;
    ss << MultiObjILS<BasicHistory, S, XMEv>::idComponent() << "BasicMOILS";
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_BasicMOILS_HPP_*/
