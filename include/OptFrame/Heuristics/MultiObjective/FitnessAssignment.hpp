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

#ifndef OPTFRAME_HEURISTICS_MULTIOBJECTIVE_FITNESSASSIGNMENT_HPP_
#define OPTFRAME_HEURISTICS_MULTIOBJECTIVE_FITNESSASSIGNMENT_HPP_

// C++
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOSIndividual.hpp>

namespace optframe {

//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class FitnessAssignment : public Component {
 public:
  FitnessAssignment() {
  }

  virtual ~FitnessAssignment() {
  }

  // assign fitness to individual 's' according to population 'P'
  virtual void assignFitnessIndividual(
      MOSIndividual<XMES2>& s,
      const vector<const MOSIndividual<XMES2>>& P) {
    vector<MOSIndividual<XMES2>> v;
    v.push_back(s);
    assignFitnessGroup(v, P);
  }

  // assign fitness to all individuals from population 'P'
  virtual void assignFitnessAll(vector<MOSIndividual<XMES2>*>& P) {
    vector<const MOSIndividual<XMES2>> Pconst(P.begin(), P.end());
    assignFitnessGroup(P, Pconst);
  }

  // assign fitness to group of individuals 'g' according to population 'P'
  virtual void assignFitnessGroup(
      vector<MOSIndividual<XMES2>>& g,
      const vector<const MOSIndividual<XMES2>>& P) = 0;

  void print() const override {
    cout << "FitnessAssignment" << endl;
  }

  string id() const override {
    return "FitnessAssignment";
  }
};

template <XEvaluation XMEv = MultiEvaluation<double>>
struct FitnessIndividual {
  int idx;
  double fitness;
  XMEv mev;

  FitnessIndividual() {
    idx = -1;
    fitness = 0;
    mev = nullptr;
  }

  FitnessIndividual(int _idx, double _fitness, XMEv _mev)
      : idx(_idx), fitness(_fitness), mev(_mev) {
  }
};

//template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class BiObjNonDominatedSort : public FitnessAssignment<XMES2> {
 public:
  using XMEv = typename XMES2::second_type;
  using XEv = typename XMEv::XEv;
  vector<Direction<XEv>> vDir;

  explicit BiObjNonDominatedSort(vector<Direction<XEv>>& _vDir)
      : vDir{_vDir} {
    assert(vDir.size() == 2);
  }

  virtual ~BiObjNonDominatedSort() {
  }

  static bool sortByFirst(
      const FitnessIndividual<XMEv>& ind1, const FitnessIndividual<XMEv>& ind2) {
    return ind1.mev->at(0).evaluation() < ind2.mev->at(0).evaluation();
  }

  static bool sortBySecond(
      const FitnessIndividual<XMEv>& ind1, const FitnessIndividual<XMEv>& ind2) {
    return ind1.mev->at(1).evaluation() < ind1.mev->at(1).evaluation();
  }

  virtual void assignFitnessGroup(
      vector<MOSIndividual<XMES2>*>& g,
      const vector<const MOSIndividual<XMES2>*>& Pop) {
    // ASSUMES UNIQUE ELEMENTS IN 'Pop'

    const int INF = 10000000;

    vector<FitnessIndividual<XMEv>> P(Pop.size());
    for (unsigned s = 0; s < P.size(); s++)
      P[s] = FitnessIndividual<XMEv>(s, INF, Pop[s]->mev);

    sort(P.begin(), P.end(), sortBySecond);        // any sort is good!
    stable_sort(P.begin(), P.end(), sortByFirst);  // necessary to be stable!!
    // (otherwise, when first objective is equal then the second may be downgraded in future and destroy dominance)

    unsigned count_x = 0;
    int rank = 0;
    while (count_x != P.size()) {
      // find first
      unsigned j = 0;
      for (j = 0; j < P.size(); j++)
        if (P[j].fitness == INF)
          break;
      P[j].fitness = rank;
      //cout << "rank: " << rank << " to ";
      //P[j]->print();
      int max_obj2 = P[j].mev->at(1).evaluation();
      int min_obj2 = max_obj2;
      //cout << "max_obj2: " << max_obj2 << " min_obj2: " << min_obj2 << endl;
      count_x++;
      vector<int> m;
      m.push_back(j);

      for (unsigned r = j + 1; r < P.size(); r++)
        if (P[r].fitness == INF) {
          if (P[r].mev->at(1).evaluation() > max_obj2)
            continue;  // discard element (is dominated!)

          if (P[r].mev->at(1).evaluation() < min_obj2)  // add element (improves the best obj2!)
          {
            //cout << "r:" << r << " ";
            P[r].fitness = rank;
            //cout << "rank: " << rank << " to ";
            //P[r]->print();
            //cout << "min_obj2: " << min_obj2 << " => ";
            min_obj2 = P[r].mev->at(1).evaluation();
            //cout << min_obj2 << endl;
            count_x++;
            m.push_back(r);
            continue;
          }

          // otherwise, check on list
          bool nonDom = true;
          for (unsigned z = 0; z < m.size(); z++)
            if (P[m[z]].mev->at(1).evaluation() <= P[r].mev->at(1).evaluation())  // <= is enough, because distance is better (and values are unique)
            {
              //cout << "r: " << r << " dominated by " << m[z] << " m=" << m << endl;
              nonDom = false;
              break;
            }

          if (nonDom) {
            //cout << "*r:" << r << " ";
            P[r].fitness = rank;
            //cout << "rank: " << rank << " to ";
            //P[r]->print();
            //cout << "m=" << m << endl;
            count_x++;
            m.push_back(r);
          } else {
            //cout << "r: " << r << " not selected! [" << P[r]->mev.at(0).evaluation() << ";" << P[r]->mev.at(1).evaluation() << "]" << endl;
          }
        }

      //cout << "FINISHED RANK " << rank << " WITH " << m << endl << endl;
      m.clear();

      rank++;
    }

    for (unsigned s = 0; s < g.size(); s++)
      for (unsigned k = 0; k < P.size(); k++)
        if (g[s]->id == Pop[P[k].idx]->id) {
          g[s]->fitness = P[k].fitness;
          break;
        }
  }
};

//template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
template <XESolution XMES2>
class NonDominatedSort : public FitnessAssignment<XMES2> {
 public:
  using XMEv = typename XMES2::second_type;
  using XEv = typename XMEv::XEv;
  vector<Direction<XEv>> vDir;

  explicit NonDominatedSort(vector<Direction<XEv>>& _vDir)
      : vDir{_vDir} {
  }

  virtual ~NonDominatedSort() {
  }

  virtual void assignFitnessGroup(
      vector<MOSIndividual<XMES2>*>& g,
      const vector<const MOSIndividual<XMES2>*>& Pop) {
    ParetoDominance<XMES2> pDominance(vDir);

    vector<FitnessIndividual<XMEv>> P(Pop.size());
    for (unsigned s = 0; s < P.size(); s++)
      P[s] = FitnessIndividual<XMEv>(s, -1, Pop[s]->mev);

    if (Component::information)
      std::cout << this->id() << "::fastNonDominatedSort |P|=";
    std::cout << P.size() << " begin" << endl;

    vector<vector<int>> F;
    F.push_back(vector<int>{});
    vector<vector<int>> S(P.size());
    vector<int> n(P.size());

    // for each 'p' in 'P'
    double sum_dom_time = 0.0;
    int count_dom_times = 0;
    for (unsigned p = 0; p < P.size(); p++) {
      S[p].clear();  // Sp = {}
      n[p] = 0;      // np = 0

      // for each 'q' in 'P'
      for (unsigned q = 0; q < P.size(); q++)
        if (p != q) {
          pair<bool, bool> v = pDominance.birelation(
              *P.at(p).mev, *P.at(q).mev);

          // if (p << q)
          if (v.first)
            S[p].push_back(q);  // Sp = Sp U {q}
          // else if (q << p)
          else if (v.second)
            n[p]++;  // np = np + 1
        }

      // if n_p = 0, p belongs to the first front
      if (n[p] == 0) {
        P[p].fitness = 0;   // p rank = 1
        F[0].push_back(p);  // F_1 = F_1 U {p}
      }
    }

    int i = 0;  // i=1
    // while Fi != {}
    while (F[i].size() != 0) {
      if (Component::debug) {
        std::cout << "fastNonDominatedSort start i=" << i;
        std::cout << " |F[i]|=" << F[i].size() << endl;
      }
      vector<int> Q;  // Q = {}

      //for each 'p' in 'Fi'
      for (unsigned k = 0; k < F[i].size(); k++) {
        int p = F[i].at(k);

        // for each 'q' in 'Sp'
        for (unsigned j = 0; j < S[p].size(); j++) {
          int q = S[p][j];

          // nq = nq - 1
          n[q]--;

          // if nq = 0
          if (n[q] == 0) {
            P[q].fitness = i + 1;  // q rank = i+1
            Q.push_back(q);        // Q = Q U {q}
          }
        }
      }

      i++;  // i = i + 1
      //F.push_back(nullptr);
      // CANNOT PUSH EMPTY ANYMORE... SHOULD THIS VECTOR BE std::optional???
      assert(false);  //F.push_back(nullptr);

      F[i] = Q;  // Fi = Q
      if (Component::debug)
        cout << "fastNonDominatedSort i=" << i << " |Q|=" << Q.size() << endl;
    }

    // finished, P is updated, update group 'g'
    for (unsigned s = 0; s < g.size(); s++)
      for (unsigned k = 0; k < P.size(); k++)
        if (g[s]->id == Pop[P[k].idx]->id) {
          g[s]->fitness = P[k].fitness;
          break;
        }
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTIOBJECTIVE_FITNESSASSIGNMENT_HPP_
