// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_MOLOCALSEARCHES_GPLS_HPP_
#define OPTFRAME_HEURISTICS_MOLOCALSEARCHES_GPLS_HPP_

// C++
#include <algorithm>
#include <vector>
//
#include "../../Concepts/BaseConcepts.hpp"
#include "../../Evaluator.hpp"
#include "../../Heuristics/MOLocalSearches/MOBestImprovement.hpp"
#include "../../InitialPareto.hpp"
#include "../../MOLocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Pareto.hpp"
#include "../../ParetoDominance.hpp"
#include "../../ParetoDominanceWeak.hpp"

namespace optframe {

template <XSolution S, XEvaluation XMEv = MultiEvaluation<>,
          XESolution XMES = pair<S, XMEv>>
struct gplsStructure {
  vector<vector<bool>> nsParetoOptimum;
  vector<bool> newSol;

  gplsStructure() {}
};

template <XESolution XES, XESolution XMES>
class ParetoManagerGPLS : public ParetoManager<XES, XMES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XMEv = typename XMES::second_type;

 private:
  int r;

 public:
  gplsStructure<S, XMEv> gplsData;
  // Pareto<XMES> x_e; //TODO -- possibility of embedding Pareto here

  ParetoManagerGPLS(sref<MultiEvaluator<XES, XMES>> _mev, int _r)
      :  // paretoManagerGPLS(GeneralEvaluator<XMES>& _mev, int _r) :
        ParetoManager<XES, XMES>(_mev),
        r(_r) {}

  virtual ~ParetoManagerGPLS() {}

  // Special addSolution used in the GPLS speedUp
  // virtual bool addSolutionWithMEV(Pareto<XMES>& p, const S& candidate,
  // const MultiEvaluation<>& candidateMev)
  virtual bool addSolutionWithMEV(Pareto<XMES>& p, const XMES& cand_smev) {
    bool added = true;
    for (unsigned ind = 0; ind < p.size(); ind++) {
      const XMEv& popIndFitness = p.getIndMultiEvaluation(ind);

      // if (paretoManager<S, XMEv, XMES>::domWeak.dominates(popIndFitness,
      // candidateMev))
      if (ParetoManager<XES, XMES>::domWeak.dominates(popIndFitness,
                                                      cand_smev.second))
        return false;

      // if (paretoManager<S, XMEv, XMES>::dom.dominates(candidateMev,
      // popIndFitness))
      if (ParetoManager<XES, XMES>::dom.dominates(cand_smev.second,
                                                  popIndFitness)) {
        p.erase(ind);
        gplsData.nsParetoOptimum.erase(gplsData.nsParetoOptimum.begin() + ind);
        gplsData.newSol.erase(gplsData.newSol.begin() + ind);
        ind--;
      }
    }

    if (added) {
      // p.add_indWithMev(candidate, candidateMev);
      p.add_indWithMev(cand_smev);
      vector<bool> neigh;
      for (int n = 0; n < r; n++) neigh.push_back(false);
      gplsData.nsParetoOptimum.push_back(neigh);
      gplsData.newSol.push_back(true);
    }

    return added;
  }
};

// General Two-Phase Pareto Local Search
//  "We call this General local search approach to multiobjective problems the
//  Pareto local search (PLS)."
//   A Dynasearch Neighborhood for the Bicriteria Traveling Salesman Problem ,
//   by Angel (2004) and Paquete(2004)
//  "Generalization in the multiobjective case of the most simple metaheuristic:
//  the hill-climbing method. "
//   Speed-up techniques for solving large-scale biobjective TSP, by Lust (2010)

// GPLS is a kind of Multi-Objective version of the VND
// However, it is designed in an efficient manner for iteratively exploring the
// obtained non-dominated solution
template <XESolution XES, XEMSolution XMES>
class GeneralParetoLocalSearch : public MOLocalSearch<XES, XMES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XMEv = typename XMES::second_type;
  static_assert(is_same<S, typename XMES::first_type>::value);

 private:
  sref<InitialPareto<XMES>> init_pareto;
  int init_pop_size;
  vsref<MOLocalSearch<XES, XMES>> vLS;
  ParetoManagerGPLS<XES, XMES> pMan2PPLS;

 public:
  GeneralParetoLocalSearch(sref<MultiEvaluator<XES, XMES>> _mev,
                           sref<InitialPareto<XMES>> _init_pareto,
                           int _init_pop_size,
                           vsref<MOLocalSearch<XES, XMES>> _vLS)
      : init_pareto(_init_pareto),
        init_pop_size(_init_pop_size),
        vLS(_vLS),
        pMan2PPLS(ParetoManagerGPLS<XES, XMES>(_mev, _vLS.size())) {}

  virtual ~GeneralParetoLocalSearch() {}

  /*
        virtual void moSearchFrom(Pareto<XMES>& p, S& s, paretoManager<S,
     XMEv, XMES>& pManager, const StopCriteria<XMEv>& stopCriteria) override
        {
                Pareto<XMES> _pf;
                pManager.addSolution(_pf,s);
                searchWithOptionalPareto(stopCriteria,&_pf);
        }
*/

  void moSearchFrom(Pareto<XMES>& p, XMES& se,
                    ParetoManager<XES, XMES>& pManager,
                    const StopCriteria<XMEv>& stopCriteria) override {
    // S& s = se.first;
    // XMEv& sMev = se.second;
    // Pareto<XMES> _pf;
    op<Pareto<XMES>> opf = make_optional(p);
    // pManager.addSolutionWithMEV(_pf,s,sMev);
    pManager.addSolutionWithMEV(*opf, se);
    // Pareto<XMES>* pr = searchWithOptionalPareto(stopCriteria,&_pf);
    // p = std::move(*pr);
    // delete pr;
    // p = searchWithOptionalPareto(stopCriteria,&_pf);
    searchWithOptionalPareto(stopCriteria, opf);
    p = *opf;
  }

  // virtual Pareto<XMES> searchWithOptionalPareto(const StopCriteria<XMEv>&
  // stopCriteria, Pareto<XMES>* _pf = nullptr)
  virtual void searchWithOptionalPareto(const StopCriteria<XMEv>& stopCriteria,
                                        op<Pareto<XMES>>& ioPF) {
    Timer tnow;

    cout << "exec: General 2PPLS (tL:" << stopCriteria.timelimit << ")" << endl;
    int r = vLS.size();

    gplsStructure<S, XMEv> gPLSData;
    Pareto<XMES> x_e;

    // if (_pf == nullptr)
    if (ioPF == std::nullopt) {
      cout << "Creating initial population using a initial pareto method:"
           << init_pop_size << endl;
      if (tnow.now() < stopCriteria.timelimit)
        x_e = init_pareto->generatePareto(init_pop_size,
                                          stopCriteria.timelimit - tnow.now());

      cout << "Population generated with " << x_e.size() << " individuals!"
           << endl;
    } else {
      // assert(_pf->size() > 0);
      assert(ioPF->size() > 0);

      cout << "Extracting Pareto Front given as parameter..." << endl;
      // x_e = std::move(*_pf); //check this move with AIIIGOR todo
      x_e = std::move(*ioPF);  // check this move with AIIIGOR todo

      cout << "Extracting PF contains " << x_e.size() << " individuals."
           << endl;
    }

    // TODO - create a new method that does not copy
    // instead, it updated p during loop and do not perform neighborhood search
    // in individuals that were dominated copy x_e to p
    Pareto<XMES> p = x_e;

    // Initializing data structures -- Pareto Optimum and NewSol (guides aux
    // population p_a)
    for (int i = 0; i < (int)x_e.size(); i++) {
      vector<bool> neigh;
      for (int n = 0; n < r; n++) neigh.push_back(false);
      pMan2PPLS.gplsData.nsParetoOptimum.push_back(neigh);
      pMan2PPLS.gplsData.newSol.push_back(true);
    }

    cout << "Number of initial x_e non-dominated solutions = " << x_e.size()
         << endl;

    int k = 0;
    cout << "Starting search with k = " << k << endl;
    while ((k < r) && (tnow.now() < stopCriteria.timelimit)) {
      for (int ind = 0; ind < (int)x_e.size(); ind++) {
        // All individuals from NS k-1 will be visited
        string localSearchId = vLS[k]->id();
        // Speed-up only if it is an exaustive search through the whole NS
        if (localSearchId == "OptFrame:MOLocalSearch:MO-BI")
          pMan2PPLS.gplsData.nsParetoOptimum[ind][k] = true;
        // Ensure that all individuals are marked as old solutions - Only the
        // new ones will be marked as true
        pMan2PPLS.gplsData.newSol[ind] = false;
      }

      // Run local search for each individual of the population - Pareto
      // Manager, pMan2PPLS, updates population
      StopCriteria<XMEv> stopCriteriaLS;
      stopCriteriaLS.timelimit = stopCriteria.timelimit;

      for (int ind = 0; ind < (int)p.size(); ind++) {
        // XMES smev = make_pair(p.getNonDominatedSol(ind),
        // p.getIndMultiEvaluation(ind));
        XMES& smev = p.getP(ind);
        vLS[k]->moSearchFrom(x_e, smev, pMan2PPLS, stopCriteriaLS);
        // p.getNonDominatedSol(ind) = smev.first;
        // p.getIndMultiEvaluation(ind) = smev.second;
      }

      //			for(int e=0;e<x_e.size();e++)
      //			{
      //				x_e.getIndMultiEvaluation(e).print();
      //			}
      //			getchar();

      p.clear();

      // TODO - This copy can be avoid and system optimized with some smart
      // strategy Updated current Pareto p with the individuals added in this
      // current iteration
      for (int ind = 0; ind < (int)x_e.size(); ind++)
        if (pMan2PPLS.gplsData.newSol[ind])
          // p.add_indWithMev(x_e.getNonDominatedSol(ind),
          // x_e.getIndMultiEvaluation(ind));
          p.add_indWithMev(x_e.getP(ind));

      if (p.size() != 0) {
        k = 0;
      } else {
        k++;
        // speed-up - Ideas from our great friend Thibaut Lust
        // Instead of delete non-optimum, only include those that should be
        // explored.
        // TODO - This copy can be avoid and system optimized with some smart
        // strategy
        // TODO CHECK THIS
        //				for (int ind = 0; ind < (int)
        // x_e.size(); ind++) 					if
        // (!pMan2PPLS.gplsData.nsParetoOptimum[ind][k]) //speed-up - Thibauuuut
        // Lust - Nice guy
        // p.add_indWithMev(x_e.getNonDominatedSol(ind),
        // x_e.getIndMultiEvaluation(ind));
        //				//end of the speed-up

        //				cout<<"Non-optimum regarding k and
        // optimals:"<<p.size()<<"/"<<x_e.size()<<endl;

        // OLD Copy/Delete - Deprecated
        p = x_e;

        if (k < r) {
          int removed = 0;
          for (int i = 0; i < (int)pMan2PPLS.gplsData.nsParetoOptimum.size();
               i++)
            if (pMan2PPLS.gplsData.nsParetoOptimum[i][k - 1]) {
              p.erase(i - removed);
              removed++;
            }
        }
      }

      cout << "p.size() = " << p.size();
      cout << "\t pMan2PPLS.x_e.size() = " << x_e.size();
      cout << "\t k = " << k << endl;
    }

    // Pareto<XMES>* pReturn = new Pareto<XMES>(std::move(x_e));
    Pareto<XMES> pReturn = std::move(x_e);
    p.clear();

    // checking possible dominance problems -- TODO - Remove for a faster code
    pMan2PPLS.checkDominance(pReturn);

    cout << "General Two-Phase Pareto Local Search finished with "
         << pReturn.size() << " non-dominated solutions." << endl;

    // return pReturn;
    ioPF = make_optional(pReturn);
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MOLOCALSEARCHES_GPLS_HPP_
