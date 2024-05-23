// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MOILSLevels_HPP_
#define OPTFRAME_MOILSLevels_HPP_

// C++
#include <algorithm>
#include <string>
#include <utility>
//

#include "./MOILSLPerturbation.hpp"
#include "./MultiObjILS.hpp"

namespace optframe {

typedef pair<pair<int, int>, pair<int, int>> levelHistory;

template <XESolution XES, XEMSolution XMES>
class MOILSLevels
    : public MultiObjILS<levelHistory, typename XMES::first_type> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XES::second_type;

 private:
  sref<MOILSLPerturbation<XMES, XMEv>> p;
  int iterMax, levelMax;

 public:
  MOILSLevels(sref<MultiEvaluator<XES, XMES>> _mev,
              sref<InitialPareto<XMES>> _init_pareto, int _init_pop_size,
              sref<MOLocalSearch<XES, XMES>> _ls, sref<RandGen> _rg,
              sref<MOILSLPerturbation<XMES, XMEv>> _p, int _iterMax,
              int _levelMax)
      :  // MOILSLevels(GeneralEvaluator<XMES>& _mev, InitialPareto<XES,
         // XMES>& _init_pareto, int _init_pop_size, MOLocalSearch<S, XMEv>*
         // _ls, RandGen& _rg, MOILSLPerturbation<XMES, XMEv>& _p, int _iterMax,
         // int _levelMax) :
        MultiObjILS<levelHistory, S, XMEv>(_mev, _init_pareto, _init_pop_size,
                                           _ls, _rg),
        p(_p),
        iterMax(_iterMax),
        levelMax(_levelMax) {}

  virtual ~MOILSLevels() {}

  levelHistory& initializeHistory() override {
    // cout << "initializeHistory()" << endl;
    pair<int, int> vars(0, 0);

    // IterMax e LevelMax
    pair<int, int> maxs(iterMax, levelMax);

    return *new levelHistory(vars, maxs);
  }

  void perturbation(XMES& smev, const StopCriteria<XMEv>& stopCriteria,
                    levelHistory& history) override {
    // cout << "perturbation(.)" << endl;

    int iter = history.first.first;
    int level = history.first.second;
    int iterMax = history.second.first;
    // int levelMax = history.second.second;

    // cout << "level = " << level << " e iter = " << iter << endl;

    // nivel atual: 'level'
    p->perturb(smev, stopCriteria, level);

    // Incrementa a iteracao
    iter++;

    if (Component::debug) cout << "MOILSL::iter " << iter << endl;

    if (iter >= iterMax) {
      iter = 0;
      level++;
      if (Component::information)
        cout << "MOILSL::level " << level << ".." << endl;
    }

    // Atualiza o historico
    history.first.first = iter;
    history.first.second = level;
  }

  void acceptanceCriterion(const Pareto<XMES>& pf,
                           levelHistory& history) override {
    if (pf.getNewNonDominatedSolutionsStatus()) {
      cout << "New Pareto size: is " << pf.size();
      cout << " on [iter " << history.first.first << " of level "
           << history.first.second << "]" << endl;
      // =======================
      //  Atualiza o historico
      // =======================
      // iter = 0
      history.first.first = 0;
      // level = 0
      history.first.second = 0;
    }
  }

  bool terminationCondition(levelHistory& history) override {
    int level = history.first.second;
    int levelMax = history.second.second;

    return (level >= levelMax);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (MultiObjSearch<XMES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << MultiObjILS<levelHistory, S, XMEv>::idComponent() << "MOILSLevels";
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MOILSLevels_HPP_*/
