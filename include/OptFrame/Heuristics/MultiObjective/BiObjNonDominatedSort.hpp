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

#ifndef OPTFRAME_HEURISTICS_MULTIOBJECTIVE_BIOBJNONDOMINATEDSORT_HPP_
#define OPTFRAME_HEURISTICS_MULTIOBJECTIVE_BIOBJNONDOMINATEDSORT_HPP_

// C++
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Heuristics/MultiObjective/FitnessAssignment.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOSIndividual.hpp>

namespace optframe {

// template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS =
// OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class BiObjNonDominatedSort : public FitnessAssignment<XMES2> {
 public:
  using XMEv = typename XMES2::second_type;
  using XEv = typename XMEv::XEv;
  vsref<Direction<XEv>> vDir;

  explicit BiObjNonDominatedSort(vsref<Direction<XEv>> _vDir) : vDir{_vDir} {
    assert(vDir.size() == 2);
  }

  virtual ~BiObjNonDominatedSort() = default;

  /*
    static bool sortByFirst(const FitnessIndividual<XMEv>& ind1,
                            const FitnessIndividual<XMEv>& ind2) {
      return ind1.mev.at(0).evaluation() < ind2.mev.at(0).evaluation();
    }

    static bool sortBySecond(const FitnessIndividual<XMEv>& ind1,
                             const FitnessIndividual<XMEv>& ind2) {
      return ind1.mev.at(1).evaluation() < ind2.mev.at(1).evaluation();
    }
  */

  // void assignFitnessGroup(vector<MOSIndividual<XMES2>>& g,
  void assignFitnessGroup(const vector<int>& g,
                          vector<MOSIndividual<XMES2>>& Pop) override {
    // ASSUMES UNIQUE ELEMENTS IN 'Pop'
    if (Component::verbose) {
      std::cout << "BiObj NSGA-II: assignFitnessGroup(";
      std::cout << "|g|:" << g.size() << " ";
      std::cout << "|Pop|:" << Pop.size() << ")" << std::endl;
    }

    // for now: we only accept groups that are equal to whole population
    assert(g.size() == Pop.size());

    // const int INF = 10000000;

    // INFINITE FITNESS: Population Size + 1? Use *100 just to make it big!
    // Note that fitness on NSGA-II is naturally limited to [0, Pop.size()]
    const int INF = Pop.size() * 100;

    // TODO: prevent creation of P and use Pop instead...
    vector<FitnessIndividual<XMEv>> P(Pop.size());
    for (unsigned s = 0; s < P.size(); s++) {
      P[s] = FitnessIndividual<XMEv>(s, INF, Pop[s].second);
      if (Pop[s].second.isOutdated() || (Pop[s].second.size() == 0)) {
        std::cout << "ERROR! MultiEvaluation is Empty or Outdated!";
        std::cout << std::endl;
        assert(false);
      }
    }

    if (Component::verbose) {
      std::cout << "BEGIN Population of FitnessIndividual";
      FitnessIndividual<XMEv>::printPopulationFitness(P);
    }

    if (Component::verbose)
      std::cout << "will sort by second(|P|:" << P.size() << ")" << std::endl;
    // any sort is good!
    // sort(P.begin(), P.end(), sortBySecond);

    auto _vDir = vDir;

    sort(P.begin(), P.end(),
         [_vDir](const FitnessIndividual<XMEv>& ind1,
                 const FitnessIndividual<XMEv>& ind2) -> bool {
           // return ind1.mev.at(1).evaluation() < ind2.mev.at(1).evaluation();
           if (_vDir[1]->isMinimization())
             return ind1.mev.at(1).evaluation() < ind2.mev.at(1).evaluation();
           else
             return ind1.mev.at(1).evaluation() > ind2.mev.at(1).evaluation();
           // return _vDir[1]->betterThan(ind1.mev.at(1), ind2.mev.at(1));
         });

    if (Component::verbose) {
      std::cout << "Sorted SECOND Population of FitnessIndividual";
      FitnessIndividual<XMEv>::printPopulationFitness(P);
    }

    if (Component::verbose)
      std::cout << "will stable_sort by first()" << std::endl;
    // necessary to be stable!!
    // (otherwise, when first objective is equal then the second may be
    // downgraded in future and destroy dominance)
    //
    // stable_sort(P.begin(), P.end(), sortByFirst);

    // TODO: create TWO LAMBDAS... one for MIN and other for MAX...
    // THIS MUST BE MUCH MORE EFFICIENT! I'm pretty sure...
    stable_sort(
        P.begin(), P.end(),
        [_vDir](const FitnessIndividual<XMEv>& ind1,
                const FitnessIndividual<XMEv>& ind2) -> bool {
          // return ind1.mev.at(0).evaluation() < ind2.mev.at(0).evaluation();
          if (_vDir[0]->isMinimization())
            return ind1.mev.at(0).evaluation() < ind2.mev.at(0).evaluation();
          else
            return ind1.mev.at(0).evaluation() > ind2.mev.at(0).evaluation();
          // return _vDir[0]->betterThan(ind1.mev.at(0), ind2.mev.at(0));
        });

    if (Component::verbose) {
      std::cout << "Sorted FIRST Population of FitnessIndividual";
      FitnessIndividual<XMEv>::printPopulationFitness(P);
    }

    if (Component::verbose) std::cout << "will set: count_x = 0" << std::endl;

    // 'count_x' counts the number of already selected solutions
    unsigned count_x = 0;
    // start non-dominated sorting with rank 0
    int rank = 0;
    while (count_x != P.size()) {
      // find first solution available to put in 'rank'
      unsigned j = 0;
      for (j = 0; j < P.size(); j++)
        if (P[j].fitness == INF) break;
      P[j].fitness = rank;
      if (Component::verbose)
        cout << "set rank " << rank << " to " << P[j] << std::endl;
      int max_obj2 = P[j].mev.at(1).evaluation();
      int min_obj2 = max_obj2;
      if (Component::verbose) {
        cout << "max_obj_second: " << max_obj2 << " ";
        cout << "min_obj_second: " << min_obj2 << endl;
      }
      if ((!vDir[0]->isMinimization()) || (!vDir[1]->isMinimization())) {
        std::cout << "ERROR! Only supports MIN MIN bi-objectives!" << std::endl;
        assert(false);
      }

      // this element is certainly non-dominated (because of objective 0)
      count_x++;
      vector<int> m;
      m.push_back(j);

      for (unsigned r = j + 1; r < P.size(); r++)
        if (P[r].fitness == INF) {
          // TODO: use nadir and ideal here, instead of min and max
          if (P[r].mev.at(1).evaluation() > max_obj2) {
            // discard element (is dominated!)
            continue;
          }

          // add element (if improves the best known second obj)
          if (P[r].mev.at(1).evaluation() < min_obj2) {
            // cout << "r:" << r << " ";
            P[r].fitness = rank;
            // cout << "rank: " << rank << " to ";
            // P[r]->print();
            // cout << "min_obj2: " << min_obj2 << " => ";
            min_obj2 = P[r].mev.at(1).evaluation();
            // cout << min_obj2 << endl;
            //
            // add another one to non-dominated front
            count_x++;
            m.push_back(r);
            continue;
          }

          // otherwise, check on list
          bool nonDom = true;
          for (unsigned z = 0; z < m.size(); z++) {
            // <= is enough, because distance is better (and values are unique)
            if (P[m[z]].mev.at(1).evaluation() <= P[r].mev.at(1).evaluation()) {
              if (Component::verbose) {
                cout << "r: " << r << " dominated by ";
                cout << m[z] << " m=" << m << endl;
              }
              nonDom = false;
              break;
            }
          }

          if (nonDom) {
            P[r].fitness = rank;
            //
            if (Component::verbose) {
              cout << "*r:" << r << " ";
              cout << "rank: " << rank << " to ";
              cout << "P[" << r << "]:" << P[r] << endl;
              cout << "m=" << m << endl;
            }
            count_x++;
            m.push_back(r);
          } else {
            if (Component::verbose) {
              cout << "r: " << r << " not selected! [";
              cout << P[r].mev.at(0).evaluation() << ";";
              cout << P[r].mev.at(1).evaluation() << "]" << endl;
            }
          }
        }

      if (Component::verbose) {
        cout << "FINISHED RANK " << rank << " WITH m=" << m << " ";
        cout << "ID_LIST on m: [";
        for (unsigned i = 0; i < m.size(); i++) cout << P[m[i]].idx << " ";
        cout << "]" << endl;
        cout << endl;
      }
      m.clear();

      rank++;
    }

    if (Component::verbose) {
      std::cout << "END Population of FitnessIndividual" << std::endl;
      FitnessIndividual<XMEv>::printPopulationFitness(P);
    }

    // TODO: this should be automatic...
    // must work with Pop instead of P...
    assert(P.size() == Pop.size());
    for (unsigned i = 0; i < P.size(); i++)
      Pop[P[i].idx].fitness = P[i].fitness;
    /*
    for (unsigned s = 0; s < g.size(); s++)
      for (unsigned k = 0; k < P.size(); k++)
        if (g[s] == Pop[P[k].idx].id) {
          g[s].fitness = P[k].fitness;
          break;
        }
    */
  }

  std::string toString() const override { return "BiObjNonDominatedSort"; }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTIOBJECTIVE_BIOBJNONDOMINATEDSORT_HPP_
