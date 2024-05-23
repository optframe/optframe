// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HELPER_POPULATIONBASEDMULTIOBJSEARCH_HPP_
#define OPTFRAME_HELPER_POPULATIONBASEDMULTIOBJSEARCH_HPP_

// c++
#include <vector>
//
#include <OptFrame/Heuristics/MultiObjective/DiversityManagement.hpp>
#include <OptFrame/Heuristics/MultiObjective/FitnessAssignment.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOElitism.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOPopulationManagement.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOSIndividual.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOSSelection.hpp>
#include <OptFrame/MultiDirection.hpp>
#include <OptFrame/MultiObjSearch.hpp>
#include <OptFrame/Timer.hpp>

#include "OptFrame/Pareto.hpp"

namespace optframe {

// template <XESolution XMES, XESolution XMES2 = XMES, XSearch<XMES2> XMSH2 =
// XMES2>

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS =
// OPTFRAME_DEFAULT_DS>

// primary base-type: XMES
//  -> output / best-type / target-type on "search" is Pareto<XMES>
// secondary base-type: XMES
//  -> search-type/incumbent-type is VEPopMOS=VEPopulation<MOSIndividual<XMES2>>

template <XESolution XMES>
using VEPopMOS = VEPopulation<MOSIndividual<XMES>>;

// this Helper is meant for Multi Objective Populational methods
// TODO: add IPopulational here (NOT NOW!)

// TODO: we COULD have XMES2 here if secondary base-type is still
// something even weirder....... could this help on ExtNSGAII?
// will leave XMES2 just as a last option...
template <XESolution XMES, XESolution XMES2 = XMES>
class PopulationBasedMultiObjSearch
    : public MultiObjSearch<XMES, MOSIndividual<XMES2>, VEPopMOS<XMES2>> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  // this is homogeneous multi-obj based on XEv obj type
  using XEv = typename XMEv::XEv;
  using MyPareto = Pareto<XMES>;

 public:
  sref<MultiDirection<XEv>> mDir;

  explicit PopulationBasedMultiObjSearch(sref<MultiDirection<XEv>> _mDir)
      : mDir{_mDir} {}

  virtual ~PopulationBasedMultiObjSearch() = default;

  virtual vector<double> initializeBounds() {
    vector<double> best(mDir->nObjectives);
    for (unsigned i = 0; i < mDir->nObjectives; i++) best[i] = mDir->nadir(i);
    return best;
  }

  // return true if any value was improved
  virtual bool updateBounds(vector<MOSIndividual<XMES2>>& P,
                            vector<double>& best) {
    bool improved = false;
    for (unsigned s = 0; s < P.size(); s++) {
      for (unsigned i = 0; i < best.size(); i++) {
        if (mDir->getDirections()[i]->betterThan(P[s].second.at(i).evaluation(),
                                                 best[i])) {
          best[i] = P[s].second.at(i).evaluation();
          improved = true;
        }
      }
    }
    return improved;
  }

  // assign fitness to subset 'g' that belongs to 'P'
  // TODO: VEPopMOS<XMES2> ?
  virtual void assignFitness(
      // vector<MOSIndividual<XMES2>>& g,
      const vector<int>& g, vector<MOSIndividual<XMES2>>& P) = 0;

  // assign diversity to subset 'g' that belongs to 'P'
  // TODO: VEPopMOS<XMES> ?
  virtual void assignDiversity(
      // vector<MOSIndividual<XMES2>>& g,
      const vector<int>& g, vector<MOSIndividual<XMES2>>& P) = 0;

  // update archive based on current population
  // TODO: VEPopMOS<XMES> ?
  virtual void updateArchive(const vector<MOSIndividual<XMES2>>& P,
                             vector<MOSIndividual<XMES2>>& archive) = 0;

  // from MultiObjSearch
  SearchOutput<XMES, Pareto<XMES>> searchBy(
      const StopCriteria<XMEv>& stopCriteria,
      std::optional<Pareto<XMES>> _best) override = 0;

  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);
    //
    mDir->setMessageLevelR(ll);
    //
    return true;
  }
};

// =======================================================================
// this Helper is meant for Multi Objective Populational methods with NS...
// WHAT IS 'NS' IN THIS CONTEXT?????
// =======================================================================

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS =
// OPTFRAME_DEFAULT_DS>

// Will LIKELY not use XMES2.... only for some WEIRD non-XMES pop type...
// Could this help with ExtNSGAII????? maybe......
template <XESolution XMES, XESolution XMES2 = XMES>
class NSPopulationBasedMultiObjSearch
    : public PopulationBasedMultiObjSearch<XMES, XMES2> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  // this is homogeneous multi-obj based on XEv obj type
  using XEv = typename XMEv::XEv;
  using XES = DEFAULT_PARETO_XES;  // WORKAROUND!
  using MyPareto = Pareto<XMES>;

 protected:
  sref<MultiEvaluator<XES, XMES>> mev;
  sref<MultiDirection<XEv>> mDir;
  sref<MOPopulationManagement<XMES>> popMan;
  unsigned popSize;

  // stopping criteria (-1: indicates unused)
  int maxGen;   // total number of generations
  int maxIter;  // generations without improvement

 public:
  NSPopulationBasedMultiObjSearch(sref<MultiEvaluator<XES, XMES>> _mev,
                                  sref<MultiDirection<XEv>> _mDir,
                                  sref<MOPopulationManagement<XMES>> _popMan,
                                  unsigned _popSize, int _maxIter,
                                  int _maxGen = 100000000)
      : PopulationBasedMultiObjSearch<XMES, XMES2>(_mDir),
        mev(_mev),
        mDir(_mDir),
        popMan(_popMan),
        popSize(_popSize),
        maxGen(_maxGen),
        maxIter(_maxIter) {}

  virtual ~NSPopulationBasedMultiObjSearch() = default;

  // evaluate and return best values for population
  virtual vector<double> evaluate(vector<MOSIndividual<XMES2>>& Pop) = 0;

  // from PopulationBasedMultiObjSearch
  void assignFitness(
      // vector<MOSIndividual<XMES2>>& g,
      const vector<int>& g, vector<MOSIndividual<XMES2>>& P) override = 0;

  // from PopulationBasedMultiObjSearch
  void assignDiversity(
      // vector<MOSIndividual<XMES2>>& g,
      const vector<int>& g, vector<MOSIndividual<XMES2>>& P) override = 0;

  // from PopulationBasedMultiObjSearch
  void updateArchive(const vector<MOSIndividual<XMES2>>& P,
                     vector<MOSIndividual<XMES2>>& archive) override = 0;

  // new method
  virtual void select(unsigned popSize, vector<MOSIndividual<XMES2>>& P,
                      vector<MOSIndividual<XMES2>>& archive) = 0;

  // new method
  virtual void freePopulation(vector<MOSIndividual<XMES2>>& P,
                              vector<MOSIndividual<XMES2>>& archive) = 0;

  virtual void printPopulation(const vector<MOSIndividual<XMES2>>& P) {
    std::cout << "NSPopulationBasedMultiObjSearch::printPopulation(|P|:";
    std::cout << P.size() << "):" << std::endl;
    for (unsigned i = 0; i < P.size(); i++)
      std::cout << "P[" << i << "]:" << P[i] << std::endl;
  }

  // Pareto<R, ADS, DS>* search(double timelimit = 100000000,
  //    double target_f = 0, Pareto<R, ADS, DS>* _pf = nullptr) {

  // from MultiObjSearch
  SearchOutput<XMES, Pareto<XMES>> searchBy(
      const StopCriteria<XMEv>& stop,
      std::optional<Pareto<XMES>> _best) override {
    Timer timer;

    if (Component::information) {
      std::cout << "NSPopulationBasedMultiObjSearch::search(";
      std::cout << "timelimit=" << stop.timelimit << ")" << std::endl;
    }

    vector<MOSIndividual<XMES2>> archive;

    if (Component::verbose)
      std::cout << "DEBUG: will gen population popSize=" << popSize
                << std::endl;

    vector<MOSIndividual<XMES2>> P = popMan->initialize(popSize);
    if (Component::verbose) {
      std::cout << "DEBUG: P.size = " << P.size() << std::endl;
      //
      std::cout << "DEBUG: will evaluate()" << std::endl;
    }
    //
    evaluate(P);

    if (Component::verbose) {
      std::cout << std::endl;
      printPopulation(P);
    }

    if (Component::verbose)
      std::cout << "DEBUG: will assignFitness()" << std::endl;
    // ignore group id parameter, for now
    std::vector<int> v_id;
    for (unsigned i = 0; i < P.size(); i++) v_id.push_back(i);
    //
    assignFitness(v_id, P);

    if (Component::verbose) {
      std::cout << std::endl;
      printPopulation(P);
      //
      std::cout << "DEBUG: will assignDiversity()" << std::endl;
    }
    // ignore group id parameter, for now
    assignDiversity(v_id, P);

    if (Component::verbose) {
      std::cout << std::endl;
      printPopulation(P);
    }

    if (Component::verbose)
      std::cout << "DEBUG: will createNext()" << std::endl;
    //
    vector<MOSIndividual<XMES2>> Q = popMan->createNext(popSize, P);
    //
    if (Component::verbose)
      std::cout << "DEBUG: Q.size = " << Q.size() << std::endl;
    //
    vector<double> bestQ = evaluate(Q);

    if (Component::verbose) {
      std::cout << std::endl;
      std::cout << "Q:" << std::endl;
      printPopulation(Q);
    }

    int t = 0;
    int tImp = 0;

    vector<double> bestObj(mDir->nObjectives);
    for (unsigned i = 0; i < bestObj.size(); i++) {
      // why initialize with worst?
      bestObj[i] = mDir->nadir(i);
      if (Component::verbose)
        std::cout << "bestObj[" << i << "] = " << bestObj[i] << std::endl;
    }

    while ((timer.now() < stop.timelimit) && (t <= maxGen) &&
           (tImp <= maxIter)) {
      //
      if (Component::verbose) {
        std::cout << "main_loop: tImp=" << tImp << "/maxIter=" << maxIter
                  << std::endl;
        std::cout << "will evaluate(|P|=" << P.size() << ")" << std::endl;
      }
      //
      vector<double> bestP = evaluate(P);
      //
      if (Component::verbose)
        std::cout << "will perform P = P U Q" << std::endl;
      //
      P.insert(P.end(), Q.begin(), Q.end());
      //
      if (Component::verbose)
        std::cout << "P.size() = " << P.size() << std::endl;
      //
      for (unsigned i = 0; i < bestP.size(); i++)
        if (mDir->betterThanAt(i, bestQ[i], bestP[i])) bestP[i] = bestQ[i];

      if (Component::verbose) {
        std::cout << std::endl << "P:" << std::endl;
        printPopulation(P);
      }

      if (Component::verbose)
        std::cout << "will assignFitness(|P|=" << P.size() << ")" << std::endl;
      // ignore group parameter for now
      std::vector<int> v_id;
      for (unsigned i = 0; i < P.size(); i++) v_id.push_back(i);
      //
      assignFitness(v_id, P);
      if (Component::verbose)
        std::cout << "will assignDiversity(|P|=" << P.size() << ")"
                  << std::endl;
      // ignore group parameter for now
      assignDiversity(v_id, P);
      //
      if (Component::verbose) {
        std::cout << "will select(popSize=" << popSize;
        std::cout << "|P|=" << P.size();
        std::cout << "|archive|=" << archive.size() << ")" << std::endl;
      }
      select(popSize, P, archive);

      if (Component::verbose) {
        std::cout << "P: " << std::endl;
        printPopulation(P);
      }

      if (Component::verbose) {
        std::cout << "archive: " << std::endl;
        printPopulation(archive);
      }

      // archive is updated
      // unused already free'd

      bool improved = false;
      for (unsigned s = 0; s < archive.size(); s++)
        for (unsigned i = 0; i < bestObj.size(); i++)
          if (mDir->betterThanAt(i, bestP[i], bestObj[i])) {
            // IMPROVEMENT IN ONE OBJECTIVE
            improved = true;
            bestObj[i] = bestP[i];
            if (Component::information) {
              std::cout << "NSPopBasedMO tImp = " << tImp << " ";
              std::cout << "Improvement in Objective " << i << ": ";
              std::cout << "bestObj[" << i << "] = " << bestObj[i] << std::endl;
            }
          }

      if (improved) {
        tImp = -1;
        if (Component::verbose) {
          // std::cout << "t=" << t << " improved bounds: " << bestObj <<
          // std::endl;
          std::cout << "t=" << t << " improved bounds: (TODO PRINT VECTOR)"
                    << std::endl;
        }
      } else {
        if (Component::verbose) {
          // std::cout << "DID NOT IMPROVE ANY BOUNDS " << bestObj << std::endl;
          std::cout << "DID NOT IMPROVE ANY BOUNDS (TODO PRINT VECTOR)"
                    << std::endl;
        }
      }

      // generating next population
      // Pconst = vector<MOSIndividual<XMES2>>(P.begin(), P.end());
      Q = popMan->createNext(popSize, P);
      bestQ = evaluate(Q);
      t++;
      tImp++;
    }

    freePopulation(P, archive);

    if (Component::verbose) {
      std::cout << "FINAL archive: " << std::endl;
      printPopulation(archive);
    }

    vsref<Direction<XEv>> vs_dir = mDir->getDirections();
    VEPopulation<XMES> finalPop;
    for (unsigned i = 0; i < archive.size(); i++)
      finalPop.push_back({archive[i].first, archive[i].second});
    //
    vector<XMES> nondom = Pareto<XMES>::filterDominated(mev, finalPop);

    if (Component::verbose) {
      std::cout << "after non domination filter:" << std::endl;
      std::cout << "SIZE = " << nondom.size() << std::endl;
    }

    Pareto<XMES> pf;
    for (unsigned i = 0; i < nondom.size(); i++) {
      S s = nondom.at(i).first;
      XMEv mev = nondom.at(i).second;
      pf.push_back({s, mev});
    }

    if (Component::verbose) {
      std::cout << "FINAL pareto: " << std::endl;
      pf.print();
    }

    SearchOutput<XMES, Pareto<XMES>> sout{SearchStatus::NO_REPORT, pf};

    return sout;
  }

 public:
  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);
    //
    mev->setMessageLevelR(ll);
    mDir->setMessageLevelR(ll);
    popMan->setMessageLevelR(ll);
    //
    return true;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HELPER_POPULATIONBASEDMULTIOBJSEARCH_HPP_
