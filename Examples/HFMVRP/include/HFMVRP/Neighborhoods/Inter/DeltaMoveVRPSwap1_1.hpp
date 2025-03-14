// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAMOVEVRPSWAP1_1_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAMOVEVRPSWAP1_1_HPP_

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
//
#include "../../ProblemInstance.hpp"
#include "../../Solution.h"
// MoveVRPSwap1_1
#include <OptFrame/Util/NSAdapter/VRP/Inter/NSSeqVRPSwap1_1.hpp>

using namespace std;

namespace HFMVRP {

// personalized move
class DeltaMoveVRPSwap1_1 : public Move<ESolutionHFMVRP> {
  /*
  class DeltaMoveVRPSwap1_1
      : public MoveVRPSwap1_1<int, AdsHFMVRP, SolutionHFMVRP> {
    typedef MoveVRPSwap1_1<int, AdsHFMVRP, SolutionHFMVRP> super;
  */
 private:
  ProblemInstance* hfmvrp;
  int vType1;
  int vType2;

  int r1, r2, c1, c2;

 public:
  DeltaMoveVRPSwap1_1(int _r1, int _r2, int _c1, int _c2,
                      ProblemInstance* _hfmvrp)
      : r1{_r1}, r2{_r2}, c1{_c1}, c2{_c2}, hfmvrp{_hfmvrp} {
    if (!_hfmvrp) {
      std::cout << "Error: hfmvrp problem is NULL!" << std::endl;
      print();
      exit(1);
    }

    if (r1 >= hfmvrp->nVehicles)
      vType1 = 2;  // MultiTripVehicle
    else
      vType1 = hfmvrp->getVehicleType(r1);

    if (r2 >= hfmvrp->nVehicles)
      vType2 = 2;  // MultiTripVehicle
    else
      vType2 = hfmvrp->getVehicleType(r2);
  }

  virtual ~DeltaMoveVRPSwap1_1() {}

  int myabs(int x) { return std::abs(x); }

  virtual bool canBeApplied(const ESolutionHFMVRP& se) {
    const SolutionHFMVRP& s = se.first;
    const RepHFMVRP& rep = s.getR();
    const AdsHFMVRP& ads = s.getADS();

    if (r1 >= 0) {
      bool all_positive = (r1 >= 0) && (r2 >= 0) && (c1 >= 1) &&
                          (c1 < (((int)rep[r1].size()) - 1)) && (c2 >= 1) &&
                          (c2 < (((int)rep[r2].size()) - 1));

      // Compatibility
      bool is_Comp = hfmvrp->getComp(vType2, rep[r1][c1]) &&
                     hfmvrp->getComp(vType1, rep[r2][c2]);

      // std::cout << " route[" << r1 << "] client " << c1 << " <=>  route[" << r2 <<
      // "] client " << c2 << ")" << std::endl; std::cout << rep[r1] << std::endl; std::cout <<
      // rep[r2] << std::endl;

      bool capR1 =
          ((hfmvrp->demands[rep[r2][c2]] - hfmvrp->demands[rep[r1][c1]] +
            ads.sumDemand[r1]) <= hfmvrp->getVehicleTypeCap(vType1));
      bool capR2 =
          ((hfmvrp->demands[rep[r1][c1]] - hfmvrp->demands[rep[r2][c2]] +
            ads.sumDemand[r2]) <= hfmvrp->getVehicleTypeCap(vType2));

      // If one of the routes is out of optimum (value 0), the variable
      // localOptimum has its value equal to 1

      // bool localOptimum =
      // !((ads.neighborhoodStatus.find(idComponent())->second)[r1] &&
      // (ads.neighborhoodStatus.find(idComponent())->second)[r2]);

      // std::cout << "hfmvrp->demands[rep[r2][c2]] = " <<
      // hfmvrp->demands[rep[r2][c2]] << " hfmvrp->demands[rep[r1][c1]]= " <<
      // hfmvrp->demands[rep[r1][c1]] << std::endl; std::cout << "ads.sumDemand[r1] = " <<
      // ads.sumDemand[r1] << " ads.sumDemand[r2] = " << ads.sumDemand[r2] <<
      // std::endl;
      /*
       if (!is_Comp)
       {
       std::cout << "Invalid Move - Incompatibility" << std::endl;
       std::cout << " route[" << r1 << "] client " << c1 << " <=>  route[" << r2 <<
       "] client " << c2 << ")"<<endl; std::cout<<rep[r1]<<endl; std::cout<<rep[r2]<<endl;
       std::cout << "vType1 = " << vType1 << "\t rep[r1][c1] = " << rep[r1][c1];
       std::cout << "\t hfmvrp->getComp(vType1, rep[r1][c1]) = " <<
       hfmvrp->getComp(vType1, rep[r1][c1]) << std::endl; std::cout << "\t
       hfmvrp->getComp(vType2, rep[r1][c1]) = " <<
       hfmvrp->getComp(vType2,rep[r1][c1]) << std::endl; std::cout << "vType2 = " <<
       vType2 << "\t rep[r2][c2] = " << rep[r2][c2];

       std::cout << "\t hfmvrp->getComp(vType2, rep[r2][c2]) = " <<
       hfmvrp->getComp(vType2, rep[r2][c2]) << std::endl; std::cout << "\t
       hfmvrp->getComp(vType1, rep[r2][c2]) = " << hfmvrp->getComp(vType1,
       rep[r2][c2]) << std::endl; getchar();
       }*/

      return all_positive && is_Comp && capR1 && capR2 &&
             (int(rep.size()) >= 2);
    } else {
      return 0;
    }
  }

  virtual void updateNeighStatus(AdsHFMVRP& ads) {
    for (map<string, vector<bool>>::iterator iter =
             ads.neighborhoodStatus.begin();
         iter != ads.neighborhoodStatus.end(); iter++) {
      ads.neighborhoodStatus[iter->first][r1] = false;
      ads.neighborhoodStatus[iter->first][r2] = false;
    }
  }

  void updateModifiedRoutes(RepHFMVRP& rep, AdsHFMVRP& ads) {
    double diffV1 = hfmvrp->demands[rep[r1][c1]] - hfmvrp->demands[rep[r2][c2]];
    double diffV2 = hfmvrp->demands[rep[r2][c2]] - hfmvrp->demands[rep[r1][c1]];

    ads.sumDemand[r1] += diffV1;
    ads.sumDemand[r2] += diffV2;

    // Cumulative Demand Calculus
    // std::cout << ads.cumulativeDemand[r1] << std::endl;
    // std::cout << ads.cumulativeDemand[r2] << std::endl;

    for (int s = c1; s < (int(rep[r1].size()) - 1); s++)
      if ((s - 1) >= 0) ads.cumulativeDemand[r1][s] += diffV1;

    for (int s = c2; s < (int(rep[r2].size()) - 1); s++)
      if ((s - 1) >= 0) ads.cumulativeDemand[r2][s] += diffV2;

    // std::cout << ads.cumulativeDemand[r1] << std::endl;
    // std::cout << ads.cumulativeDemand[r2] << std::endl;
    // getchar();
    // minDemand and MaxDemand Calculus
    vector<int> modRoutes;  // modified routes
    modRoutes.push_back(r1);
    modRoutes.push_back(r2);

    for (int iter = 0; iter < 2; iter++) {
      int r = modRoutes[iter];
      vector<int> route = rep[r];
      int routeSize = route.size();

      if (routeSize >= 3) {
        // two depots plus one client
        vector<double> routeDemands(routeSize);

        routeDemands[0] = 0;              // depot demand
        routeDemands[routeSize - 1] = 0;  // depot demand

        for (int c = 1; c < int(routeSize - 1); c++)
          routeDemands[c] = hfmvrp->demands[route[c]];

        // minPairDemand and maxPairDemand Calculus
        double minPairDemand = 100000000;
        double maxPairDemand = 0;
        for (int i = 1; i < int(routeSize - 1); i++) {
          if (i != int(routeSize - 2)) {
            double pairDemand = routeDemands[i] + routeDemands[i + 1];
            if (pairDemand < minPairDemand) minPairDemand = pairDemand;
            if (pairDemand > maxPairDemand) maxPairDemand = pairDemand;
          }
        }
        if (maxPairDemand == 0) {
          minPairDemand = 0;
          maxPairDemand = 0;
        }

        ads.maxPairDemand[r] = maxPairDemand;
        ads.minPairDemand[r] = minPairDemand;

        // minDemand and maxDemand Calculus
        std::vector<std::pair<int, double>> rankDemands;
        for (int i = 1; i < int(routeSize - 1); i++)
          rankDemands.push_back(std::make_pair(i, routeDemands[i]));

        sort(rankDemands.begin(), rankDemands.end(), compare);
        // std::cout << rankDemands << std::endl;

        ads.maxDemand[r] = rankDemands[0].second;
        ads.minDemand[r] = rankDemands[rankDemands.size() - 1].second;
      }
    }
  }

  uptr<Move<ESolutionHFMVRP>> apply(ESolutionHFMVRP& se) override {
    SolutionHFMVRP& s = se.first;
    RepHFMVRP& rep = s.getR();
    AdsHFMVRP& ads = s.getADS();
    // std::cout << "Aplicando Delta Move: route[" << r1 << "] client " << c1 << "
    // <=>  route[" << r2 << "] client " << c2 << ")";

    int c;
    c = rep.at(r1).at(c1);
    rep.at(r1).at(c1) = rep.at(r2).at(c2);
    rep.at(r2).at(c2) = c;

    // Update ADS
    /*
     for (int i = 0; i < rep.size(); i++)
     {
     std::cout << "V[" << i << "]";
     for (int j = 0; j < rep[i].size(); j++)
     std::cout << rep[i][j] << ";";
     }

     std::cout << "\n r1 = " << r1 << "\tr2 = " << r2 << std::endl;
     */

    updateNeighStatus(ads);

    updateModifiedRoutes(rep, ads);

    /*
     for (map<string, vector<bool> >::iterator iter =
     ads.neighborhoodStatus.begin(); iter != ads.neighborhoodStatus.end();
     iter++)
     {
     std::cout << "iter->first = " << iter->first << std::endl;
     std::cout << "iter->second = " << iter->second << std::endl;
     }
     getchar();
     */

    return uptr<Move<ESolutionHFMVRP>>(
        new DeltaMoveVRPSwap1_1(r2, r1, c2, c1, hfmvrp));
  }

  op<EvaluationHFMVRP> cost(const pair<SolutionHFMVRP, Evaluation<>>& se,
                            bool allowEstimated) override {
    const SolutionHFMVRP& s = se.first;
    const RepHFMVRP& rep = s.getR();
    const AdsHFMVRP& ads = s.getADS();

    vector<int> route1 = rep[r1];
    vector<int> route2 = rep[r2];

    // int routeSize1 = route1.size();
    // int routeSize2 = route2.size();

    double vTDC1 = hfmvrp->getVehicleTypeDistCost(vType1);
    double vTDC2 = hfmvrp->getVehicleTypeDistCost(vType2);

    unsigned k1 = c1;
    unsigned k2 = c2;

    // before k1 and k2
    unsigned bk1 = k1 - 1;
    unsigned bk2 = k2 - 1;

    // after k1 and k2
    unsigned ak1 = k1 + 1;
    unsigned ak2 = k2 + 1;

    double f = 0;

    f -= hfmvrp->getDist(route1[bk1], route1[k1]) * vTDC1;
    // std::cout << "-d(" << route[bk1] << "," << route[k1] << ") \t f= " << f <<
    // std::endl;
    f -= hfmvrp->getDist(route1[k1], route1[ak1]) * vTDC1;
    // std::cout << "-d(" << route[k1] << "," << route[ak1] << ") \t f= " << f <<
    // std::endl;
    f -= hfmvrp->getDist(route2[bk2], route2[k2]) * vTDC2;
    // std::cout << "-d(" << route[bk2] << "," << route[k2] << ") \t f= " << f <<
    // std::endl;
    f -= hfmvrp->getDist(route2[k2], route2[ak2]) * vTDC2;
    // std::cout << "-d(" << route[k2] << "," << route[ak2] << ") \t f= " << f <<
    // std::endl;

    f += hfmvrp->getDist(route1[bk1], route2[k2]) * vTDC1;
    // std::cout << "+d(" << route[bk1] << "," << route[k2] << ") \t f= " << f <<
    // std::endl;
    f += hfmvrp->getDist(route2[k2], route1[ak1]) * vTDC1;
    // std::cout << "+d(" << route[k2] << "," << route[ak1] << ") \t f= " << f <<
    // std::endl;
    f += hfmvrp->getDist(route2[bk2], route1[k1]) * vTDC2;
    // std::cout << "+d(" << route[bk2] << "," << route[k1] << ") \t f= " << f <<
    // std::endl;
    f += hfmvrp->getDist(route1[k1], route2[ak2]) * vTDC2;
    // std::cout << "+d(" << route[k1] << "," << route[ak2] << ") \t f= " << f <<
    // std::endl;

    double fo_v1_inv = (hfmvrp->demands[rep[r2][c2]] -
                        hfmvrp->demands[rep[r1][c1]] + ads.sumDemand[r1]) -
                       hfmvrp->getVehicleTypeCap(vType1);
    double fo_v2_inv = (hfmvrp->demands[rep[r1][c1]] -
                        hfmvrp->demands[rep[r2][c2]] + ads.sumDemand[r2]) -
                       hfmvrp->getVehicleTypeCap(vType2);

    if (fo_v1_inv < 0) fo_v1_inv = 0;
    if (fo_v2_inv < 0) fo_v2_inv = 0;

    /// return new MoveCost<>(f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  static std::string idComponent() {
    string idComp = Move<ESolutionHFMVRP>::idComponent();
    idComp.append(":DeltaMoveVRPSwap1_1");
    return idComp;
  }

  virtual bool operator==(const Move<ESolutionHFMVRP>& _m) const {
    const DeltaMoveVRPSwap1_1& m1 = (const DeltaMoveVRPSwap1_1&)_m;
    return ((m1.r1 == r1) && (m1.r2 == r2) && (m1.c1 == c1) && (m1.c2 == c2));
  }

  void print() const override {
    std::cout << "DeltaMoveVRPSwap1_1( ";
    std::cout << " route[" << r1 << "] client " << c1 << " <=>  route[" << r2
         << "] client " << c2 << ")";
    std::cout << std::endl;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAMOVEVRPSWAP1_1_HPP_
