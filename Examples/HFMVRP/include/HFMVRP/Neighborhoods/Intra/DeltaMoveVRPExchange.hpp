// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRPEXCHANGE_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRPEXCHANGE_HPP_

#include <cmath>
//
#include <OptFrame/Util/NSAdapter/VRP/Intra/NSSeqVRPExchange.hpp>

using namespace std;

namespace HFMVRP {

class DeltaMoveVRPExchange
    : public MoveVRPExchange<ESolutionHFMVRP>  //<int, AdsHFMVRP,
                                               // SolutionHFMVRP>
{
  typedef MoveVRPExchange<ESolutionHFMVRP> super;

 private:
  ProblemInstance* hfmvrp;

 public:
  DeltaMoveVRPExchange(
      vector<vector<int>>& (*_getRoutes)(const ESolutionHFMVRP& s), int _r,
      int _c1, int _c2, ProblemInstance* _hfmvrp)
      : super(_getRoutes, _r, _c1, _c2), hfmvrp(_hfmvrp) {
    if (!_hfmvrp) {
      cout << "Error: hfmvrp problem is NULL!" << endl;
      print();
      exit(1);
    }
  }

  virtual ~DeltaMoveVRPExchange() {}

  int myabs(int x) { return std::abs(x); }

  virtual bool canBeApplied(const ESolutionHFMVRP& se) {
    const SolutionHFMVRP& s = se.first;
    const RepHFMVRP& rep = s.getR();
    if (r >= 0) {
      bool all_positive = (c1 >= 1) && (c1 < (((int)rep[r].size()) - 1)) &&
                          (c2 >= 1) && (c2 < (((int)rep[r].size()) - 1)) &&
                          (r >= 0);

      return all_positive &&
             (rep.at(r).size() >= 5);  // asymmetric or symmetric todo If
                                       // asymmetric =>rep.at(r).size() >=4
    } else {
      return 0;
    }
  }

  virtual void updateNeighStatus(AdsHFMVRP& ads) {
    for (map<string, vector<bool>>::iterator iter =
             ads.neighborhoodStatus.begin();
         iter != ads.neighborhoodStatus.end(); iter++) {
      ads.neighborhoodStatus[iter->first][r] = false;
    }
  }

  uptr<Move<ESolutionHFMVRP>> apply(ESolutionHFMVRP& se) override {
    SolutionHFMVRP& s = se.first;
    RepHFMVRP& rep = s.getR();
    AdsHFMVRP& ads = s.getADS();
    int small, bigger;
    if (c1 <= c2) {
      small = c1;
      bigger = c2;
    } else {
      small = c2;
      bigger = c1;
    }

    int aux = rep.at(r).at(c1);
    rep.at(r).at(c1) = rep.at(r).at(c2);
    rep.at(r).at(c2) = aux;

    // Update ADS
    updateNeighStatus(ads);

    for (int s = small; s < bigger; s++) {
      if ((s - 1) >= 0)
        ads.cumulativeDemand[r][s] =
            ads.cumulativeDemand[r][s - 1] + hfmvrp->demands[rep[r][s]];
    }

    // minPairDemand and maxPairDemand Calculus
    double minPairDemand = 100000000;
    double maxPairDemand = 0;
    for (int i = 1; i < int(rep[r].size() - 1); i++) {
      if (i != int(rep[r].size() - 2)) {
        double pairDemand =
            hfmvrp->demands[rep[r][i]] + hfmvrp->demands[rep[r][i + 1]];
        if (pairDemand < minPairDemand) minPairDemand = pairDemand;
        if (pairDemand > maxPairDemand) maxPairDemand = pairDemand;
      }
    }
    if (maxPairDemand == 0) {
      minPairDemand = rep[r][1];
      maxPairDemand = rep[r][1];
    }

    ads.maxPairDemand[r] = maxPairDemand;
    ads.minPairDemand[r] = minPairDemand;

    return uptr<Move<ESolutionHFMVRP>>(
        new DeltaMoveVRPExchange(super::getRoutes, r, c1, c2, hfmvrp));
  }

  op<EvaluationHFMVRP> cost(const pair<SolutionHFMVRP, Evaluation<>>& se,
                            bool allowEstimated) override {
    const SolutionHFMVRP& s = se.first;
    const RepHFMVRP& rep = s.getR();
    // const AdsHFMVRP& ads = s.getADS();
    vector<int> route = rep[r];
    // int routeSize = route.size();
    // cout << "routeSize = " << routeSize << endl;
    // cout << route << endl;
    // getchar();
    int vType = -1;
    if (r >= hfmvrp->nVehicles)
      vType = 2;  // MultiTripVehicle
    else
      vType = hfmvrp->getVehicleType(r);

    double vTDC = hfmvrp->getVehicleTypeDistCost(vType);

    unsigned k1 = c1;
    unsigned k2 = c2;

    // before k1 and k2
    unsigned bk1 = k1 - 1;
    unsigned bk2 = k2 - 1;

    // after k1 and k2
    unsigned ak1 = k1 + 1;
    unsigned ak2 = k2 + 1;

    double f = 0;

    if (myabs(k1 - k2) >= 2) {
      f -= hfmvrp->getDist(route[bk1], route[k1]) * vTDC;
      // cout << "-d(" << route[bk1] << "," << route[k1] << ") \t f= " << f <<
      // endl;
      f -= hfmvrp->getDist(route[k1], route[ak1]) * vTDC;
      // cout << "-d(" << route[k1] << "," << route[ak1] << ") \t f= " << f <<
      // endl;
      f -= hfmvrp->getDist(route[bk2], route[k2]) * vTDC;
      // cout << "-d(" << route[bk2] << "," << route[k2] << ") \t f= " << f <<
      // endl;
      f -= hfmvrp->getDist(route[k2], route[ak2]) * vTDC;
      // cout << "-d(" << route[k2] << "," << route[ak2] << ") \t f= " << f <<
      // endl;

      f += hfmvrp->getDist(route[bk1], route[k2]) * vTDC;
      // cout << "+d(" << route[bk1] << "," << route[k2] << ") \t f= " << f <<
      // endl;
      f += hfmvrp->getDist(route[k2], route[ak1]) * vTDC;
      // cout << "+d(" << route[k2] << "," << route[ak1] << ") \t f= " << f <<
      // endl;
      f += hfmvrp->getDist(route[bk2], route[k1]) * vTDC;
      // cout << "+d(" << route[bk2] << "," << route[k1] << ") \t f= " << f <<
      // endl;
      f += hfmvrp->getDist(route[k1], route[ak2]) * vTDC;
      // cout << "+d(" << route[k1] << "," << route[ak2] << ") \t f= " << f <<
      // endl;
    } else {
      if (k1 < k2) {
        f -= hfmvrp->getDist(route[bk1], route[k1]) * vTDC;
        // cout << "-d(" << route[bk1] << "," << route[k1] << ") \t f= " << f <<
        // endl;

        f -= hfmvrp->getDist(route[k2], route[ak2]) * vTDC;
        // cout << "-d(" << route[k2] << "," << route[ak2] << ") \t f= " << f <<
        // endl;

        f += hfmvrp->getDist(route[bk1], route[k2]) * vTDC;
        // cout << "+d(" << route[bk1] << "," << route[k2] << ") \t f= " << f <<
        // endl;
        f += hfmvrp->getDist(route[k1], route[ak2]) * vTDC;
        // cout << "+d(" << route[k1] << "," << route[ak2] << ") \t f= " << f <<
        // endl;
      } else {
        f -= hfmvrp->getDist(route[k1], route[ak1]) * vTDC;
        // cout << "-d(" << route[k1] << "," << route[ak1] << ") \t f= " << f <<
        // endl;

        f -= hfmvrp->getDist(route[bk2], route[k2]) * vTDC;
        // cout << "-d(" << route[bk2] << "," << route[k2] << ") \t f= " << f <<
        // endl;

        f += hfmvrp->getDist(route[bk2], route[k1]) * vTDC;
        // cout << "+d(" << route[bk2] << "," << route[k1] << ") \t f= " << f <<
        // endl;
        f += hfmvrp->getDist(route[k2], route[ak1]) * vTDC;
        // cout << "+d(" << route[k2] << "," << route[ak1] << ") \t f= " << f <<
        // endl;
      }
    }
    // cout << endl;

    // return new MoveCost<> (f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  std::string id() const override {
    stringstream ssIdRoute;
    ssIdRoute << super::idComponent() << ":DeltaMoveVRPExchange"
              << "+Route=" << r;
    return ssIdRoute.str();
  }

  static string idComponent() {
    string idComp = super::idComponent();
    idComp.append(":DeltaMoveVRPExchange");
    return idComp;
  }

  virtual bool operator==(const Move<ESolutionHFMVRP>& _m) const {
    const DeltaMoveVRPExchange& m1 = (const DeltaMoveVRPExchange&)_m;
    return ((m1.c1 == c1) && (m1.c2 == c2) && (m1.r == r));
  }

  void print() const override {
    cout << "DeltaMoveVRPExchange( route: " << r << " : ";
    cout << " client " << c1 << " <=>  client" << c2 << " )";
    cout << endl;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRPEXCHANGE_HPP_
