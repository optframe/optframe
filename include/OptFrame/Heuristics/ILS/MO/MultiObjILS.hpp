// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MULTIOBJECTIVEILS_HPP_
#define OPTFRAME_MULTIOBJECTIVEILS_HPP_

#include <algorithm>
//

#include "../../../InitialPareto.hpp"
#include "../../../MOLocalSearch.hpp"
#include "../../../MultiEvaluator.hpp"
#include "../../../MultiObjSearch.hpp"
#include "../../../NSSeq.hpp"
#include "../../../Pareto.hpp"
#include "MOILS.h"

namespace optframe {

template <class H, XSolution S, XEvaluation XMEv = MultiEvaluation<>,
          XESolution XMES = pair<S, XMEv>, XSearch<XMES> XSH = Pareto<XMES>>
class MultiObjILS : public MOILS, public MultiObjSearch<XMES> {
  using XEv = typename XMEv::XEv;
  using XES = std::pair<S, XEv>;  // TODO:  remove

 private:
  sref<InitialPareto<XMES>> init_pareto;
  int init_pop_size;
  sref<MOLocalSearch<XES, XMES>> ls;
  ParetoManager<XES, XMES> pMan;
  sref<RandGen> rg;

 public:
  MultiObjILS(sref<MultiEvaluator<XES, XMES>> _mev,
              sref<InitialPareto<XMES>> _init_pareto, int _init_pop_size,
              sref<MOLocalSearch<XES, XMES>> _ls, sref<RandGen> _rg)
      : init_pareto(_init_pareto),
        init_pop_size(_init_pop_size),
        ls(_ls),
        pMan(ParetoManager<XES, XMES>(_mev)),
        rg(_rg) {}

  ~MultiObjILS() override {}

  virtual H& initializeHistory() = 0;

  virtual void perturbation(XMES& smev, const StopCriteria<XMEv>& stopCriteria,
                            H& history) = 0;

  virtual void acceptanceCriterion(const Pareto<XMES>& pf, H& history) = 0;

  virtual bool terminationCondition(H& history) = 0;

  SearchOutput<XMES, Pareto<XMES>> searchBy(
      const StopCriteria<XMEv>& stopCriteria,
      std::optional<Pareto<XMES>> _best) override {
    // std::optional<Pareto<XMES>>& p = this->best;
    // TODO: reimplement with SearchBy...
    std::optional<Pareto<XMES>> p;
    //
    Timer tnow;
    Pareto<XMES> x_e;
    cout << "exec: MOILS (tL:" << stopCriteria.timelimit << ")" << endl;

    // if (_pf == nullptr) {
    if (p == nullopt) {
      if (Component::information)
        cout << "Creating initial population using a initial pareto method:"
             << init_pop_size << endl;

      if (tnow.now() < stopCriteria.timelimit)
        x_e = init_pareto->generatePareto(init_pop_size,
                                          stopCriteria.timelimit - tnow.now());

      if (Component::information)
        cout << "Population generated with " << x_e.size() << " individuals!"
             << endl;
    } else {
      assert(p->size() > 0);
      if (Component::information)
        cout << "Extracting Pareto Front given as parameter..." << endl;

      // x_e = std::move(*_pf); //check this move with AIIIGOR todo
      x_e = std::move(*p);  // Igor: VITORRRRR this is even worse now! hahaha
                            // need to check!!
      p = nullopt;          // disengage!

      if (Component::information)
        cout << "Extracting PF contains " << x_e.size() << " individuals."
             << endl;
    }

    //		vector<bool> visited;
    //		for (int ind = 0; ind < x_e.size(); ind++)
    //			visited.push_back(false);

    H* history = &initializeHistory();

    while (tnow.now() < stopCriteria.timelimit &&
           !terminationCondition(*history)) {
      int ind = rg->rand(x_e.size());
      //			while (visited[ind])
      //				ind = rg.rand(x_e.size());
      //			visited[ind] = true;

      S rS = x_e.getNonDominatedSol(ind);
      MultiEvaluation<> rMev = x_e.getIndMultiEvaluation(ind);

      StopCriteria<XMEv> stopCriteriaPert;
      stopCriteriaPert.timelimit = stopCriteria.timelimit;
      //
      XMES smes = make_pair(rS, rMev);
      perturbation(smes, stopCriteriaPert, *history);
      rS = smes.first;
      rMev = smes.second;

      // Try to add the neighbor solution that was obtained from the
      // perturbation pMan.addSolutionWithMEV(x_e, rS, rMev);
      pMan.addSolutionWithMEV(x_e, smes);

      StopCriteria<XMEv> stopCriteriaLS;
      stopCriteriaLS.timelimit = stopCriteria.timelimit;
      //
      smes = make_pair(rS, rMev);
      ls->moSearchFrom(x_e, smes, pMan, stopCriteriaLS);
      rS = smes.first;
      rMev = smes.second;

      acceptanceCriterion(x_e, *history);
      x_e.setNewNonDominatedSolutionsStatus(false);

      // This is undoubtedly not working properly - TODO
      // Visited vize should be updated. It may have been deleted
      //			bool allVisited = true;
      //			for (int v = 0; v < D.size(); v++)
      //			{
      //				if (!visited[v])
      //				{
      //					allVisited = false;
      //					break;
      //				}
      //			}
      //
      //			if (allVisited)
      //				for (int v = 0; v < D.size(); v++)
      //					visited[v] = false;
    }

    ////Pareto<XMES>* pReturn = new Pareto<XMES>(std::move(x_e));
    p = make_optional(std::move(x_e));  // TODO: check if this 'move assign' is
                                        // O(1) for Pareto, as expected

    // checking possible dominance problems -- TODO - Remove for a faster code
    // pMan.checkDominance(*pReturn);
    pMan.checkDominance(*p);

    // cout << "MOILS finished with " << pReturn->size() << " non-dominated
    // solutions." << endl;
    cout << "MOILS finished with " << p->size() << " non-dominated solutions."
         << endl;

    delete history;
    // return pReturn;
    // this->best = p;
    return SearchOutput<XMES, Pareto<XMES>>{SearchStatus::NO_REPORT,
                                            p};  // nothing to say
  }
};
}  // namespace optframe

#endif /*OPTFRAME_MULTIOBJECTIVEILS_HPP_*/
