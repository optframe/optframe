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

#ifndef OPTFRAME_HEURISTICS_MULTIOBJECTIVE_CLASSICNSGAII_HPP_
#define OPTFRAME_HEURISTICS_MULTIOBJECTIVE_CLASSICNSGAII_HPP_

// c++
#include <vector>
//
#include <OptFrame/Helper/PopulationBasedMultiObjSearch.hpp>
#include <OptFrame/Heuristics/MultiObjective/BiObjNonDominatedSort.hpp>
#include <OptFrame/Heuristics/MultiObjective/NonDominatedSort.hpp>

namespace optframe {

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS =
// OPTFRAME_DEFAULT_DS>
//
template <XESolution XMES>
class ClassicNSGAII : public NSPopulationBasedMultiObjSearch<XMES> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XMEv::XEv;

 protected:
  //
  sref<MultiEvaluator<XMES>> mevr;
  sptr<MultiDirection<XEv>> mdir;
  //
  sref<FitnessAssignment<XMES>> fa;
  sref<DiversityManagement<XMES>> dm;
  sref<MOSSelection<XMES>> sel;

 public:
  ClassicNSGAII(sref<MultiEvaluator<XMES>> _mevr,
                // sref<MultiDirection<XEv>> _mDir,
                sref<MOPopulationManagement<XMES>> _popMan, unsigned popSize,
                int maxIter, int maxGen = 100000000)
      : NSPopulationBasedMultiObjSearch<XMES>(
            _mevr,
            // _mDir,
            sref<MultiDirection<XEv>>{new MultiDirection(_mevr->vDir)}, _popMan,
            popSize, maxIter, maxGen),
        mevr{_mevr},
        mdir{sptr<MultiDirection<XEv>>{new MultiDirection(mevr->vDir)}},
        fa{setupFA()},
        dm{new CrowdingDistance<XMES>(mdir->getDirections())},
        sel{new NSGAIISelection<XMES>} {
    std::cout << "ClassicNSGAII::nObjectives: ";
    std::cout << mevr->nObjectives << std::endl;
  }

  sref<FitnessAssignment<XMES>> setupFA() {
    // ===================================================
    // MIN-MIN optimization (no MAX-MAX for the moment...)
    //
    if ((this->mevr->vDir.size() == 2) &&
        this->mevr->vDir[0]->isMinimization() &&
        this->mevr->vDir[1]->isMinimization()) {
      if (Component::information)
        std::cout << "ClassicNSGAII: 2 MIN-MIN objectives (fast "
                     "BiObjNonDominatedSort)"
                  << std::endl;
      return sref<FitnessAssignment<XMES>>{
          new BiObjNonDominatedSort<XMES>(this->mdir->getDirections())};
    } else {
      if (Component::information)
        std::cout << "ClassicNSGAII: multiple objectives with NonDominatedSort"
                  << std::endl;
      return sref<FitnessAssignment<XMES>>{
          new NonDominatedSort<XMES>{this->mevr}};
    }
  }

  virtual ~ClassicNSGAII() = default;

  using NSPopulationBasedMultiObjSearch<XMES>::search;

  vector<double> evaluate(vector<MOSIndividual<XMES>>& P) override {
    vector<double> best(mevr->nObjectives);
    for (unsigned i = 0; i < best.size(); i++) best[i] = mdir->nadir(i);

    for (unsigned s = 0; s < P.size(); s++) {
      // check if not evaluated (nullptr)
      if ((P[s].second.size() == 0) || P[s].second.isOutdated()) {
        P[s].second = mevr->evaluate(P[s].first);
        for (unsigned i = 0; i < best.size(); i++)
          if (mdir->getDirections()[i]->betterThan(
                  P[s].second.at(i).evaluation(), best[i]))
            best[i] = P[s].second.at(i).evaluation();
      }
    }

    return best;
  }

  // void assignFitness(vector<MOSIndividual<XMES>>& g,
  void assignFitness(const vector<int>& g,
                     vector<MOSIndividual<XMES>>& P) override {
    fa->assignFitnessGroup(g, P);
  }

  // void assignDiversity(vector<MOSIndividual<XMES>>& g,
  void assignDiversity(const vector<int>& g,
                       vector<MOSIndividual<XMES>>& P) override {
    dm->assignDiversityGroup(g, P);
  }

  void updateArchive(const vector<MOSIndividual<XMES>>& P,
                     vector<MOSIndividual<XMES>>& archive) override {
    // NO ARCHIVING IN NSGA-II
  }

  void select(unsigned popSize, vector<MOSIndividual<XMES>>& P,
              vector<MOSIndividual<XMES>>& archive) override {
    sel->select(popSize, P, archive);
  }

  void freePopulation(vector<MOSIndividual<XMES>>& P,
                      vector<MOSIndividual<XMES>>& archive) override {
    sel->free(P, archive);
  }

 public:
  bool setVerboseR() override {
    this->setVerbose();
    //
    mevr->setVerboseR();
    mdir->setVerboseR();
    fa->setVerboseR();
    dm->setVerboseR();
    sel->setVerboseR();
    //
    return true;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTIOBJECTIVE_CLASSICNSGAII_HPP_
