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
#include <OptFrame/MultiObjSearch.hpp>
#include <OptFrame/Timer.hpp>

namespace optframe {

// template <XESolution XMES, XESolution XMES2 = XMES, XSearch<XMES2> XMSH2 = XMES2>

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>

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
class PopulationBasedMultiObjSearch : public MultiObjSearch<
                                          XMES, MOSIndividual<XMES2>,
                                          VEPopMOS<XMES2>> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  // this is homogeneous multi-obj based on XEv obj type
  using XEv = typename XMEv::XEv;

 public:
  sref<MultiDirection<XEv>> mDir;

  explicit PopulationBasedMultiObjSearch(sref<MultiDirection<XEv>> _mDir)
      : mDir{_mDir} {
  }

  virtual ~PopulationBasedMultiObjSearch() {
  }

  virtual vector<double> initializeBounds() {
    vector<double> best(mDir->nObjectives);
    for (unsigned i = 0; i < mDir->nObjectives; i++)
      best[i] = mDir->nadir(i);
    return best;
  }

  // return true if any value was improved
  virtual bool updateBounds(vector<MOSIndividual<XMES2>>& P,
                            vector<double>& best) {
    bool improved = false;
    for (unsigned s = 0; s < P.size(); s++) {
      for (unsigned i = 0; i < best.size(); i++) {
        if (mDir->getDirections()[i]->betterThan(
                P[s].second.at(i).evaluation(),
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
      vector<MOSIndividual<XMES2>>& g,
      const vector<MOSIndividual<XMES2>>& P) = 0;

  // assign diversity to subset 'g' that belongs to 'P'
  // TODO: VEPopMOS<XMES> ?
  virtual void assignDiversity(
      vector<MOSIndividual<XMES2>>& g,
      const vector<MOSIndividual<XMES2>>& P) = 0;

  // update archive based on current population
  // TODO: VEPopMOS<XMES> ?
  virtual void updateArchive(
      const vector<MOSIndividual<XMES2>>& P,
      vector<MOSIndividual<XMES2>>& archive) = 0;

  // from MultiObjSearch
  SearchOutput<XMES, Pareto<XMES>> search(
      const StopCriteria<XMEv>& stopCriteria) override = 0;
};

// =======================================================================
// this Helper is meant for Multi Objective Populational methods with NS...
// WHAT IS 'NS' IN THIS CONTEXT?????
// =======================================================================

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>

// Will LIKELY not use XMES2.... only for some WEIRD non-XMES pop type...
// Could this help with ExtNSGAII????? maybe......
template <XESolution XMES, XESolution XMES2 = XMES>
class NSPopulationBasedMultiObjSearch : public PopulationBasedMultiObjSearch<
                                            XMES, XMES2> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  // this is homogeneous multi-obj based on XEv obj type
  using XEv = typename XMEv::XEv;

 protected:
  sref<MultiDirection<XEv>> mDir;
  sref<MOPopulationManagement<XMES>> popMan;
  unsigned popSize;

  // stopping criteria (-1: indicates unused)
  int maxGen;   // total number of generations
  int maxIter;  // generations without improvement

 public:
  NSPopulationBasedMultiObjSearch(
      sref<MultiDirection<XEv>> _mDir,
      sref<MOPopulationManagement<XMES>> _popMan,
      unsigned _popSize, int _maxIter, int _maxGen = 100000000)
      : PopulationBasedMultiObjSearch<XMES, XMES2>(_mDir),
        mDir(_mDir),
        popMan(_popMan),
        popSize(_popSize),
        maxGen(_maxGen),
        maxIter(_maxIter) {
  }

  virtual ~NSPopulationBasedMultiObjSearch() {
  }

  // evaluate and return best values for population
  virtual vector<double> evaluate(vector<MOSIndividual<XMES2>>& Pop) = 0;

  // from PopulationBasedMultiObjSearch
  void assignFitness(
      vector<MOSIndividual<XMES2>>& g,
      const vector<MOSIndividual<XMES2>>& P) override = 0;

  // from PopulationBasedMultiObjSearch
  void assignDiversity(
      vector<MOSIndividual<XMES2>>& g,
      const vector<MOSIndividual<XMES2>>& P) override = 0;

  // from PopulationBasedMultiObjSearch
  void updateArchive(
      const vector<MOSIndividual<XMES2>>& P,
      vector<MOSIndividual<XMES2>>& archive) override = 0;

  // new method
  virtual void select(unsigned popSize,
                      vector<MOSIndividual<XMES2>>& P,
                      vector<MOSIndividual<XMES2>>& archive) = 0;

  // new method
  virtual void freePopulation(vector<MOSIndividual<XMES2>>& P,
                              vector<MOSIndividual<XMES2>>& archive) = 0;

  // Pareto<R, ADS, DS>* search(double timelimit = 100000000,
  //    double target_f = 0, Pareto<R, ADS, DS>* _pf = nullptr) {

  // from MultiObjSearch
  SearchOutput<XMES, Pareto<XMES>> search(
      const StopCriteria<XMEv>& stop) override {
    Timer timer;

    cout << "Population Based Multi Objective Search search()" << endl;

    vector<MOSIndividual<XMES2>> archive;

    vector<MOSIndividual<XMES2>> P = popMan->initialize(popSize);
    evaluate(P);
    vector<MOSIndividual<XMES2>> Pconst(P.begin(), P.end());
    assignFitness(P, Pconst);
    assignDiversity(P, Pconst);

    vector<MOSIndividual<XMES2>> Q = popMan->createNext(popSize, Pconst);
    vector<double> bestQ = evaluate(Q);

    int t = 0;
    int tImp = 0;

    vector<double> bestObj(mDir->nObjectives);
    for (unsigned i = 0; i < bestObj.size(); i++)
      bestObj[i] = mDir->nadir(i);

    while ((timer.now() < stop.timelimit) &&
           (t <= maxGen) && (tImp <= maxIter)) {
      vector<double> bestP = evaluate(P);
      P.insert(P.end(), Q.begin(), Q.end());
      for (unsigned i = 0; i < bestP.size(); i++)
        if (mDir->betterThanAt(i, bestQ[i], bestP[i]))
          bestP[i] = bestQ[i];

      Pconst = vector<MOSIndividual<XMES2>>(P.begin(), P.end());
      assignFitness(P, Pconst);
      assignDiversity(P, Pconst);

      select(popSize, P, archive);

      // archive is updated
      // unused already free'd

      bool improved = false;
      for (unsigned s = 0; s < archive.size(); s++)
        for (unsigned i = 0; i < bestObj.size(); i++)
          if (mDir->betterThanAt(i, bestP[i], bestObj[i])) {
            // IMPROVEMENT IN ONE OBJECTIVE
            improved = true;
            bestObj[i] = bestP[i];
          }

      if (improved) {
        tImp = -1;
        cout << "t=" << t << " improved bounds: " << bestObj << endl;
      }

      // generating next population
      Pconst = vector<MOSIndividual<XMES2>>(P.begin(),
                                            P.end());
      Q = popMan->createNext(popSize, Pconst);
      bestQ = evaluate(Q);
      t++;
      tImp++;
    }

    freePopulation(P, archive);

    Pareto<XMES> pf;  //= new Pareto<R, ADS, DS>;
    for (unsigned i = 0; i < archive.size(); i++) {
      // Solution<R, ADS>* s = archive.at(i)->s;
      S s = archive.at(i).first;
      // MultiEvaluation<DS>* mev = archive.at(i)->mev;
      XMEv mev = archive.at(i).second;
      pf.push_back({s, mev});
    }

    // delete &P;
    // delete &archive;

    return SearchOutput<XMES, Pareto<XMES>>{.status = SearchStatus::NO_REPORT, .best = pf};
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HELPER_POPULATIONBASEDMULTIOBJSEARCH_HPP_
