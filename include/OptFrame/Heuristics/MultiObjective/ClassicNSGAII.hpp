#pragma once

#include <OptFrame/Helper/PopulationBasedMultiObjSearch.hpp>

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ClassicNSGAII : public NSPopulationBasedMultiObjSearch<R, ADS, DS> {
 protected:
  MultiEvaluator<R, ADS, DS>& mevr;
  FitnessAssignment<R, ADS, DS>* fa;
  DiversityManagement<R, ADS, DS>* dm;
  MOSSelection<R, ADS, DS>* sel;

 public:
  ClassicNSGAII(MultiEvaluator<R, ADS, DS>& _mevr, PopulationManagement<R, ADS, DS>& _popMan, unsigned popSize, int maxIter, int maxGen = 100000000)
      : NSPopulationBasedMultiObjSearch<R, ADS, DS>(_mevr, _popMan, popSize, maxIter, maxGen), mevr(_mevr) {
    if (mevr.nObjectives == 2)
      fa = new BiObjNonDominatedSort<R, ADS, DS>(mevr.getDirections());
    else
      fa = new NonDominatedSort<R, ADS, DS>(mevr.getDirections());

    dm = new CrowdingDistance<R, ADS, DS>(mevr.getDirections());

    sel = new NSGAIISelection<R, ADS, DS>;
  }

  virtual ~ClassicNSGAII() {
    delete fa;
    delete dm;
    delete sel;
  }

  using NSPopulationBasedMultiObjSearch<R, ADS, DS>::search;

  vector<double> evaluate(vector<MOSIndividual<XMES2>*>& P) {
    vector<double> best(mevr.nObjectives);
    for (unsigned i = 0; i < best.size(); i++)
      best[i] = mevr.nadir(i);

    for (unsigned s = 0; s < P.size(); s++)
      if (!P[s]->mev)  // not evaluated (nullptr)
      {
        P[s]->mev = &mevr.evaluate(*P[s]->s);
        for (unsigned i = 0; i < best.size(); i++)
          if (mevr.betterThan(i, P[s]->mev->at(i).evaluation(), best[i]))
            best[i] = P[s]->mev->at(i).evaluation();
      }

    return best;
  }

  virtual void assignFitness(vector<MOSIndividual<XMES2>*>& g, const vector<const MOSIndividual<XMES2>*>& P) {
    fa->assignFitnessGroup(g, P);
  }

  virtual void assignDiversity(vector<MOSIndividual<XMES2>*>& g, const vector<const MOSIndividual<XMES2>*>& P) {
    dm->assignDiversityGroup(g, P);
  }

  virtual void updateArchive(const vector<const MOSIndividual<XMES2>*>& P, vector<MOSIndividual<XMES2>*>& archive) {
    // NO ARCHIVING IN NSGA-II
  }

  virtual void select(unsigned popSize, vector<MOSIndividual<XMES2>*>& P, vector<MOSIndividual<XMES2>*>& archive) {
    sel->select(popSize, P, archive);
  }

  virtual void freePopulation(vector<MOSIndividual<XMES2>*>& P, vector<MOSIndividual<XMES2>*>& archive) {
    sel->free(P, archive);
  }
};

}  // namespace optframe