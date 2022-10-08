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

#ifndef OPTFRAME_HEURISTICS_EA_NSGAII_HPP_
#define OPTFRAME_HEURISTICS_EA_NSGAII_HPP_

// C++
#include <algorithm>
#include <utility>
#include <vector>
//
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/Helper/InitialMultiESolution.hpp>
#include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/MultiObjSearch.hpp>
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/ParetoDominance.hpp>

namespace optframe {

constexpr int INFINITO = 1'000'000'000;

template <XESolution XMES>  //, XEvaluation XEv = Evaluation<>>
struct IndividualNSGAII {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::first_type;
  using first_type = S;
  using second_type = XMEv;
  //
  // Solution<R, ADS>& s;
  // MultiEvaluation<XEv>* mev;  // TODO: remove?
  //
  S first;      // s
  XMEv second;  // mev
  //
  int rank;
  double distance;

  bool isChild;
  bool isRandom;
  bool isCross;
  bool isMutation;

  int id;            // for debug reasons
  int num_children;  // for tests

  explicit IndividualNSGAII(S& _s)  // Solution<R, ADS>& _s)
      : first{_s} {
    // mev = nullptr;
    second.invalidate();
    //
    rank = -1;
    distance = -1;
    id = -1;
    num_children = -1;

    isChild = false;
    isCross = false;
    isRandom = false;
    isMutation = false;
  }

  // (const IndividualNSGAII<R, ADS>& ind)
  IndividualNSGAII(const IndividualNSGAII<XMES>& ind)
      : first{ind.first} {  // s(ind.s.clone()) {
    // mev = nullptr;
    second.invalidate();
    //
    // if (ind.mev)
    //  mev = &ind.mev->clone();
    if (!ind.second.isOutdated())
      second = ind.second;

    rank = ind.rank;
    distance = ind.distance;

    isChild = ind.isChild;
    id = ind.id;
    num_children = ind.num_children;

    isRandom = ind.isRandom;
    isCross = ind.isCross;
    isMutation = ind.isMutation;
  }

  virtual ~IndividualNSGAII() {
    // delete &s;
    // if (mev)
    //   delete mev;
  }

  void print() const {
    cout << "IndNSGAII: rank=" << rank << "\tdist=" << distance;
    cout << "\t[ ";
    // if (mev)
    if (!second.isOutdated()) {
      for (unsigned e = 0; e < second.size(); e++)
        cout << second.at(e).evaluation() << " ; ";
    }
    cout << " ]";

    std::cout << "\tisChild=" << isChild << " Crs=" << isCross;
    std::cout << " Rdm=" << isRandom << " Mut=" << isMutation;
    std::cout << "\t |x|=" << num_children << endl;
    // cout << s.getR() << endl;
  }

  IndividualNSGAII<XMES>& clone() const {
    // no need for this anymore
    assert(false);
    return *new IndividualNSGAII<XMES>(*this);
  }

  static void printResume(const vector<IndividualNSGAII<XMES>*>& v) {
    int count_child = 0;
    int count_cross = 0;
    int count_random = 0;
    int count_mutation = 0;
    for (unsigned i = 0; i < v.size(); i++) {
      if (v[i]->isChild)
        count_child++;
      if (v[i]->isRandom)
        count_random++;
      if (v[i]->isCross)
        count_cross++;
      if (v[i]->isMutation)
        count_mutation++;
    }
    cout << "|S|=" << v.size();
    cout << "\tchild=" << count_child;
    std::cout << "(" << (count_child * 100 / v.size()) << "%) |";
    cout << "\trandom=" << count_random;
    std::cout << "(" << (count_random * 100 / v.size()) << "%)";
    cout << "\tcross=" << count_cross;
    std::cout << "(" << (count_cross * 100 / v.size()) << "%)";
    cout << "\tmutation=" << count_mutation;
    std::cout << "(" << (count_mutation * 100 / v.size()) << "%)";
    cout << endl;
  }
};

// : public MultiObjSearch<R, ADS> {
template <XESolution XMES>
class NSGAII : public MultiObjSearch<XMES,
                                     IndividualNSGAII<XMES>,
                                     VEPopulation<IndividualNSGAII<XMES>>> {
  // typedef vector<Evaluation<>*> FitnessValues;
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XMEv::XEv;
  // Why?
  using XES = std::pair<S, XEv>;

 private:
  // vector of (single-obj) evaluators
  vsref<Evaluator<S, XEv, XES>> v_e;

  sref<InitialMultiESolution<XMES>> init_epop;
  int init_pop_size;

  sref<ParetoDominance<XMES>> pDominance;
  int gMax;

  static bool compare(pair<double, int> p1, pair<double, int> p2) {
    return p1.first > p2.first;
  }

 protected:
  sref<RandGen> rg;

 public:
  // using Heuristic<R, ADS >::exec; // prevents name hiding

  NSGAII(vsref<Evaluator<S, XEv, XES>> _v_e,
         sref<InitialMultiESolution<XMES>> _init_epop,
         int _init_pop_size,
         int _gMax,
         sref<RandGen> _rg)
      : v_e(_v_e),
        init_epop(_init_epop),
        init_pop_size(_init_pop_size),
        pDominance{new ParetoDominance<XMES>{_v_e}},
        rg(_rg) {
    // TODO(igormcoelho): why put again the evaluators? seems wrong...
    // assert(false);
    // pDominance->insertEvaluators(_v_e);
    //
    gMax = _gMax;
  }

  virtual ~NSGAII() {
  }

  virtual void basicGeneticOperators(VEPopulation<XMES>& p) = 0;

  SearchOutput<XMES, Pareto<XMES>> search(
      const StopCriteria<XMEv>& stop) override {
    Timer tnow;

    cout << "exec: Non Sorting Genetic Algorithm Search " << endl;

    VEPopulation<XMES> p = init_epop->generateEPopulation(init_pop_size);

    int N = p.size();

    VEPopulation<XMES> q = p;
    basicGeneticOperators(q);

    int g = 0;
    while ((g <= gMax) && (tnow.now() < stop.timelimit)) {
      cout << "Generation = " << g << endl;

      VEPopulation<XMES> r = p;

      for (int i = 0; i < (int)q.size(); i++)
        r.push_back(q.at(i));

      // Start NonDominance Order by sets
      vector<vector<XMES>> F = nonDominanceOrder(r);

      VEPopulation<XMES> popTemp;
      int j = 0;

      vector<double> cD;  // Crowding Distance

      while ((int)(popTemp.size() + F[j].size()) < N) {
        crowdingDistanceOrder(cD, F[j]);

        for (int i = 0; i < (int)F[j].size(); i++)
          popTemp.push_back(F[j].at(i));
        j++;
      }

      vector<double> cDTemp;
      crowdingDistanceOrder(cDTemp, F[j]);

      vector<pair<double, int>> cDOrdenated;
      for (int i = 0; i < (int)cDTemp.size(); i++)
        cDOrdenated.push_back(make_pair(cDTemp[i], i));

      sort(cDOrdenated.begin(), cDOrdenated.end(), compare);

      int popTempSize = popTemp.size();
      for (int i = 0; i < (N - popTempSize); i++) {
        cD.push_back(cDOrdenated[i].first);
        popTemp.push_back(F[j].at(cDOrdenated[i].second));
      }

      p.clear();
      p = popTemp;
      popTemp.clear();

      q.clear();
      q = basicSelection(p, cD);

      basicGeneticOperators(q);

      for (int i = 0; i < (int)F.size(); i++)
        F[i].clear();

      r.clear();

      g++;
    }

    // TODO(igormcoelho): NOW, efficiency comes later!
    // we may std::move many things here... think about it!

    // TODO: support direct migration from VEPopulation to Pareto
    std::optional<Pareto<XMES>> pf = Pareto<XMES>{};
    for (unsigned i = 0; i < p.size(); i++) {
      // TODO: move?
      XMES se = p.at(i);
      pf->push_back(se);
    }
    SearchOutput<XMES, Pareto<XMES>> sout{SearchStatus::NO_REPORT, pf};
    return sout;
  }

  void crowdingDistanceOrder(vector<double>& CD, const vector<XMES>& Fj) {
    int N = Fj.size();
    if (N > 0) {
      int CDOldSize = CD.size();
      for (int i = 0; i < N; i++)
        CD.push_back(0);

      for (int m = 0; m < (int)v_e.size(); m++) {
        vector<pair<double, int>> fitness;

        for (int i = 0; i < N; i++) {
          XEv e = v_e[m]->evaluate(Fj.at(i).first);
          fitness.push_back(make_pair(e.evaluation(), i));
          // delete &e;
        }

        sort(fitness.begin(), fitness.end(), compare);

        CD[CDOldSize + fitness[0].second] = INFINITO;
        CD[CDOldSize + fitness[N - 1].second] = INFINITO;

        for (int i = (CDOldSize + 1); i < (CDOldSize + N - 1); i++) {
          if ((fitness[0].first - fitness[N - 1].first) < 0.000001)
            CD[CDOldSize + fitness[i].second] = INFINITO;
          else
            CD[CDOldSize + fitness[i].second] =
                CD[CDOldSize + fitness[i].second] +
                (fitness[i - 1].first - fitness[i + 1].first) /
                    (fitness[0].first - fitness[N - 1].first);
        }
      }
    }
    // finish crowdingDistanceOrder
  }

  // nonDominanceOrder(F,p)
  // This method LIKELY (I don't remember well...) sorts
  // p population into fronts F... It would be nice for
  // F elements to be "reference wrappers" of elements
  // in p... this way, costs could be reduced!
  //
  vector<vector<XMES>> nonDominanceOrder(
      const VEPopulation<XMES>& p) {
    // store front elements in F
    vector<vector<XMES>> F;
    //
    VEPopulation<XMES> pAtual = p;
    // TODO: see if we can improving things with std::move
    // NO MORE POINTERS HERE!

    // F0: non-dominated front
    VEPopulation<XMES> F0;
    F.push_back(F0);

    vector<int> nd;
    vector<int> moved_s;

    for (int i = 0; i < (int)pAtual.size(); i++) {
      int nd = 0;

      for (int j = 0; j < (int)pAtual.size(); j++) {
        if (j != i) {
          // check if 'j' dominates 'i'
          if (pDominance->dominates(pAtual.at(j).second, pAtual.at(i).second))
            nd++;
        }
      }

      // check if 'nd' (non-dominated) count is zero
      if (nd == 0) {
        // TODO: std::move here or use some sort of ref_wrap
        // F[0].push_back(std::move(pAtual.at(i)));
        F[0].push_back(pAtual.at(i));
        //
        moved_s.push_back(i);
      }
    }

    int nMax = p.size() / 2;
    int nAtual = F[0].size();

    for (int i = 0; i < (int)moved_s.size(); i++)
      // delete &pAtual.remove(deleteds[i] - i);
      pAtual.erase(pAtual.begin() + (moved_s[i] - i));
    moved_s.clear();

    int k = 0;
    while ((F[k].size() != 0) && (nAtual <= nMax)) {
      k++;

      VEPopulation<XMES> uTemp;  // = new Population<XES>;
      F.push_back(uTemp);

      for (int i = 0; i < (int)pAtual.size(); i++) {
        int nd = 0;

        for (int j = 0; j < (int)pAtual.size(); j++) {
          if (j != i) {
            // check if 'j' dominates 'i'
            if (pDominance->dominates(pAtual.at(j).second, pAtual.at(i).second))
              nd++;
          }
        }

        // check if 'nd' (non-dominated) count is zero
        if (nd == 0) {
          F[k].push_back(pAtual.at(i));
          moved_s.push_back(i);
        }
      }

      for (int i = 0; i < (int)moved_s.size(); i++)
        pAtual.erase(pAtual.begin() + (moved_s[i] - i));

      nAtual += F[k].size();

      moved_s.clear();
    }
    // return fronts
    return F;
  }

  virtual VEPopulation<XMES> basicSelection(
      const VEPopulation<XMES>& p, vector<double> cD) {
    VEPopulation<XMES> q;
    for (int i = 0; i < (int)p.size(); i++) {
      int j = rg->rand(p.size());
      while (i == j)
        j = rg->rand(p.size());

      bool A = pDominance->dominates(p.at(i).second, p.at(j).second);
      if (A)
        q.push_back(p.at(i));

      bool B = pDominance->dominates(p.at(j).second, p.at(i).second);
      if (B)
        q.push_back(p.at(j));

      if (A == B) {
        if (cD[i] >= cD[j])
          q.push_back(p.at(i));
        else
          q.push_back(p.at(j));
      }
    }

    return q;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_NSGAII_HPP_
