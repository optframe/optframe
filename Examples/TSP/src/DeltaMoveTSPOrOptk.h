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

#ifndef TSP_DELTA_MOVETSP_OROPTK_HPP_
#define TSP_DELTA_MOVETSP_OROPTK_HPP_

#include <OptFrame/Util/NeighborhoodStructures/Moves/MoveTSPOrOptk.hpp>
#include <cmath>

#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace TSP {

class DeltaMoveTSPOrOptk : public MoveTSPOrOptk<int> {
  typedef MoveTSPOrOptk<int> super;

 private:
  sref<ProblemInstance> tsp;

 public:
  DeltaMoveTSPOrOptk(int i, int j, int k, sref<ProblemInstance> _tsp)
      : super(i, j, k, _tsp.sptr()), tsp(_tsp) {
    /*
              if(!tsp)
              {
                      cout << "Error: tsp problem is NULL!" << endl;
                      print();
                      exit(1);
              }
    */
  }

  virtual ~DeltaMoveTSPOrOptk() {}

  int myabs(int x) { return std::abs(x); }

  /*
       virtual bool canBeApplied(const RepTSP& rep, const OPTFRAME_DEFAULT_ADS&)
       {
               bool all_positive = (p1 >= 0) && (p2 >= 0);
               return all_positive && (rep.size() >= 2);
       }
       */

  /*
       virtual Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& apply(Route& rep,
     OPTFRAME_DEFAULT_ADS&)
       {
               reverse(rep.begin() + p1, rep.begin() + p2);

               // r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
               return *new MoveTSP2Opt(p1, p2);
       }
       */

  // op<EvaluationTSP> cost(const Evaluation<>&, const RepTSP& rep, const
  // OPTFRAME_DEFAULT_ADS& ads) override {
  op<EvaluationTSP> cost(const ESolutionTSP& se, bool allowEstimated) override {
    // const Evaluation<>& e = se.second;
    const RepTSP& rep = se.first.getR();
    // const OPTFRAME_DEFAULT_ADS& ads = se.first.getADS();
    // before i and j
    int bi = i - 1;
    int bj = j - 1;

    if (bi < 0) bi = rep.size() - 1;
    if (bj < 0) bj = rep.size() - 1;

    // after j
    unsigned aj = j + 1;

    if (aj == rep.size()) aj = 0;

    // last i+k
    unsigned lik = i + k - 1;

    // after i+k
    unsigned aik = i + k;

    if (aik == rep.size()) aik = 0;

    double f = 0;

    if (((i == ((int)rep.size()) - 1) && (j == 0)) ||
        ((i == 0 && (j == ((int)rep.size()) - 1))))  // extreme point
    {
      f = 0;
    } else {
      if (i < j) {
        f -= (*tsp->dist)(rep[bi], rep[i]);
        // cout << "-d(" << rep[bi] << "," << rep[i] << ") ";
        f -= (*tsp->dist)(rep[lik], rep[aik]);
        // cout << "-d(" << rep[lik] << "," << rep[aik] << ") ";
        f -= (*tsp->dist)(rep[j], rep[aj]);
        // cout << "-d(" << rep[j] << "," << rep[aj] << ") ";

        f += (*tsp->dist)(rep[bi], rep[aik]);
        // cout << "+d(" << rep[bi] << "," << rep[aik] << ") ";
        f += (*tsp->dist)(rep[j], rep[i]);
        // cout << "+d(" << rep[j] << "," << rep[i] << ") ";
        f += (*tsp->dist)(rep[lik], rep[aj]);
        // cout << "+d(" << rep[lik] << "," << rep[aj] << ") ";
      } else  // i > j
      {
        f -= (*tsp->dist)(rep[bi], rep[i]);
        // cout << "-d(" << rep[bi] << "," << rep[i] << ") ";
        f -= (*tsp->dist)(rep[lik], rep[aik]);
        // cout << "-d(" << rep[lik] << "," << rep[aik] << ") ";
        f -= (*tsp->dist)(rep[bj], rep[j]);
        // cout << "-d(" << rep[bj] << "," << rep[j] << ") ";

        f += (*tsp->dist)(rep[bi], rep[aik]);
        // cout << "+d(" << rep[bi] << "," << rep[aik] << ") ";
        f += (*tsp->dist)(rep[bj], rep[i]);
        // cout << "+d(" << rep[bj] << "," << rep[i] << ") ";
        f += (*tsp->dist)(rep[lik], rep[j]);
        // cout << "+d(" << rep[lik] << "," << rep[j] << ") ";
      }
    }

    // cout << endl;

    // return new MoveCost<>(f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  static string idComponent() {
    string idComp = super::idComponent();
    idComp.append(":DeltaMoveTSPOrOptk");
    return idComp;
  }

  bool operator==(const Move<ESolutionTSP>& _m) const override {
    const DeltaMoveTSPOrOptk& m1 = (const DeltaMoveTSPOrOptk&)_m;
    return (m1.i == i) && (m1.j == j) && (m1.k == k);
  }

  void print() const override {
    cout << "DeltaMoveTSPOrOpt{K=" << k << "}";
    cout << "(" << i << ";" << j << ")" << endl;
  }
};

}  // namespace TSP

#endif /*TSP_DELTA_MOVETSP_OROPTK_HPP_*/
