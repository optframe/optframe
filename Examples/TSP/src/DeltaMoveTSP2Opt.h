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

#ifndef TSP_DELTA_MOVETSP2OPT_HPP_
#define TSP_DELTA_MOVETSP2OPT_HPP_

#include <OptFrame/Util/NeighborhoodStructures/Moves/MoveTSP2Opt.hpp>
#include <cmath>

#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace TSP {

class DeltaMoveTSP2Opt : public MoveTSP2Opt<int, OPTFRAME_DEFAULT_ADS> {
  typedef MoveTSP2Opt<int, OPTFRAME_DEFAULT_ADS> super;

 private:
  sref<ProblemInstance> tsp;

 public:
  DeltaMoveTSP2Opt(int p1, int p2, sref<ProblemInstance> _tsp)
      : super(p1, p2, _tsp.sptr()), tsp(_tsp) {
    /*
              if(!tsp)
              {
                      cout << "Error: tsp problem is NULL!" << endl;
                      print();
                      exit(1);
              }
    */
  }

  virtual ~DeltaMoveTSP2Opt() {}

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

  op<EvaluationTSP> cost(const ESolutionTSP& se, bool estimated) override {
    // const Evaluation<>& e = se.second;
    const RepTSP& rep = se.first.getR();
    // const OPTFRAME_DEFAULT_ADS& ads = se.first.getADS();
    if (p1 >= p2) {
      cout << "DeltaMoveTSP2Opt FATAL ERROR!" << endl;
      exit(1);
    }
    unsigned k1 = p1;
    unsigned k2 = p2;

    if (k2 == rep.size()) k2 = 0;

    // before k1 and k2
    unsigned bk1 = k1 - 1;
    unsigned bk2 = k2 - 1;

    if (k1 == 0) bk1 = rep.size() - 1;
    if (k2 == 0) bk2 = rep.size() - 1;

    double f = 0;

    if (myabs(k1 - k2) >= 2) {
      f -= (*tsp->dist)(rep[bk1], rep[k1]);
      // cout << "-d(" << rep[bk1] << "," << rep[k1] << ") ";
      f -= (*tsp->dist)(rep[bk2], rep[k2]);
      // cout << "-d(" << rep[bk2] << "," << rep[k2] << ") ";

      f += (*tsp->dist)(rep[bk1], rep[bk2]);
      // cout << "+d(" << rep[bk1] << "," << rep[bk2] << ") ";
      f += (*tsp->dist)(rep[k1], rep[k2]);
      // cout << "+d(" << rep[k1] << "," << rep[k2] << ") ";
    }
    // cout << endl;

    // return new MoveCost<>(f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  static string idComponent() {
    string idComp = super::idComponent();
    idComp.append(":DeltaMoveTSP2Opt");
    return idComp;
  }

  bool operator==(const Move<ESolutionTSP>& _m) const override {
    const DeltaMoveTSP2Opt& m1 = (const DeltaMoveTSP2Opt&)_m;
    return ((m1.p1 == p1) && (m1.p2 == p2));
  }

  void print() const override {
    cout << "DeltaMoveTSP2Opt( ";
    cout << " edge " << p1 << " <=>  edge " << p2 << " )";
    cout << endl;
  }
};

}  // namespace TSP

#endif /*TSP_DELTA_MOVETSP2OPT_HPP_*/
