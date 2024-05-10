// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAMOVEVRPCROSS_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAMOVEVRPCROSS_HPP_

#include <cmath>
//
#include <OptFrame/Util/NSAdapter/VRP/Inter/NSSeqVRPCross.hpp>

using namespace std;

namespace HFMVRP {

class DeltaMoveVRPCross
    : public MoveVRPCross<int, AdsHFMVRP, OPTFRAME_DEFAULT_DS> {
  typedef MoveVRPCross<int, AdsHFMVRP, OPTFRAME_DEFAULT_DS> super;

 private:
  ProblemInstance* hfmvrp;

 public:
  DeltaMoveVRPCross(int _r1, int _r2, int _p1, int _p2,
                    ProblemInstance* _hfmvrp)
      : super(_r1, _r2, _p1, _p2), hfmvrp(_hfmvrp) {
    if (!_hfmvrp) {
      cout << "Error: hfmvrp problem is NULL!" << endl;
      print();
      exit(1);
    }
  }

  virtual ~DeltaMoveVRPCross() {}

  int myabs(int x) { return std::abs(x); }

  /*
   virtual bool canBeApplied(const RepHFMVRP& rep, const AdsHFMVRP&)
   {
   bool all_positive = (p1 >= 1) && p1 < (rep[r].size() - 1) && (p2 >= 1) && p2
   < (rep[r].size() - 1); bool minSize = (rep[r].size() >= 3);



   return all_positive && minSize;
   }

   /*
   virtual Move<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& apply(Route& rep,
   OPTFRAME_DEFAULT_ADS&)
   {
   reverse(rep.begin() + p1, rep.begin() + p2);

   // r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
   return *new MoveTSP2Opt(p1, p2);
   }
   */

  pair<double, double>* cost(const pair<SolutionHFMVRP, Evaluation<>>& se,
                             const AdsHFMVRP& ads) {
    double f = 0;

    return new pair<double, double>(f, 0);
  }

  static string idComponent() {
    string idComp = super::idComponent();
    idComp.append(":DeltaMoveVRP2Opt");
    return idComp;
  }

  virtual bool operator==(const Move<RepHFMVRP, AdsHFMVRP>& _m) const {
    const DeltaMoveVRPCross& m1 = (const DeltaMoveVRPCross&)_m;
    return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.p1 == p1) && (m1.p2 == p2));
  }

  void print() const override {
    cout << "DeltaMoveVRPCross( ";
    cout << r1 << ", " << r2 << ", " << p1 << ", " << p2 << " )";
    cout << endl;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAMOVEVRPCROSS_HPP_
