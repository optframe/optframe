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

#ifndef OPTFRAME_HEURISTICS_MULTIOBJECTIVE_NONDOMINATEDSORT_HPP_
#define OPTFRAME_HEURISTICS_MULTIOBJECTIVE_NONDOMINATEDSORT_HPP_

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
class NonDominatedSort : public FitnessAssignment<XMES2> {
 public:
  using XMEv = typename XMES2::second_type;
  using XEv = typename XMEv::XEv;

  sref<MultiEvaluator<XMES2>> mev;
  vsref<Direction<XEv>> vDir;

  /*
  explicit NonDominatedSort(vsref<Direction<XEv>> _vDir)
      : vDir{_vDir} {
  }
  */

  explicit NonDominatedSort(sref<MultiEvaluator<XMES2>> _mev)
      : mev{_mev}, vDir{_mev->vDir} {
    assert(mev->vDir.size() > 0);
  }

  virtual ~NonDominatedSort() = default;

  void assignFitnessGroup(
      // vector<MOSIndividual<XMES2>>& g,
      const vector<int>& g, vector<MOSIndividual<XMES2>>& Pop) override {
    ParetoDominance<XMES2> pDominance{mev};
    //
    // for now, ignore 'g' (assume only "ALL" can be executed here!)
    assert(g.size() == Pop.size());

    // initialize individuals with -1 fitness
    // (TODO: reuse MOSIndividual structure)
    vector<FitnessIndividual<XMEv>> P(Pop.size());
    for (unsigned s = 0; s < P.size(); s++)
      P[s] = FitnessIndividual<XMEv>(s, -1, Pop[s].second);

    if (Component::information)
      std::cout << this->id() << "::fastNonDominatedSort |P|=";
    std::cout << P.size() << " begin" << endl;

    // Initialize pareto front vector F (of ids)
    vector<vector<int>> F;
    F.push_back(vector<int>{});
    // ???
    vector<vector<int>> S(P.size());
    vector<int> n(P.size());

    // for each 'p' in 'P'
    // double sum_dom_time = 0.0;
    // int count_dom_times = 0;
    for (unsigned p = 0; p < P.size(); p++) {
      S[p].clear();  // Sp = {}
      n[p] = 0;      // np = 0

      // for each 'q' in 'P'
      for (unsigned q = 0; q < P.size(); q++)
        if (p != q) {
          pair<bool, bool> v = pDominance.birelation(P.at(p).mev, P.at(q).mev);

          // if (p << q)
          if (v.first) S[p].push_back((int)q);  // Sp = Sp U {q}
          // else if (q << p)
          else if (v.second)
            n[p]++;  // np = np + 1
        }

      // if n_p = 0, p belongs to the first front
      if (n[p] == 0) {
        P[p].fitness = 0;        // p rank = 1
        F[0].push_back((int)p);  // F_1 = F_1 U {p}
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

      // for each 'p' in 'Fi'
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
      // F.push_back(nullptr);
      //  CANNOT PUSH EMPTY ANYMORE... SHOULD THIS VECTOR BE std::optional???
      assert(false);  // F.push_back(nullptr);

      F[i] = Q;  // Fi = Q
      if (Component::debug)
        cout << "fastNonDominatedSort i=" << i << " |Q|=" << Q.size() << endl;
    }

    /*
    // finished, P is updated, update group 'g'
    for (unsigned s = 0; s < g.size(); s++)
      for (unsigned k = 0; k < P.size(); k++)
        if (g[s].id == Pop[P[k].idx].id) {
          g[s].fitness = P[k].fitness;
          break;
        }
    */
  }

  std::string toString() const override { return "NonDominatedSort"; }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTIOBJECTIVE_NONDOMINATEDSORT_HPP_
