// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRP2OPT_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRP2OPT_HPP_

#include <cmath>
//
#include <OptFrame/Util/NSAdapter/VRP/Intra/NSSeqVRP2Opt.hpp>

using namespace std;

namespace HFMVRP {

// class DeltaMoveVRP2Opt: public MoveVRP2Opt<int, AdsHFMVRP, SolutionHFMVRP>
//
// class DeltaMoveVRP2Opt: public MoveVRP2Opt<vector<vector<int>>, AdsHFMVRP,
// SolutionHFMVRP>
class DeltaMoveVRP2Opt : public MoveVRP2Opt<ESolutionHFMVRP> {
  // typedef MoveVRP2Opt<vector<vector<int>>, AdsHFMVRP, SolutionHFMVRP> super;
  typedef MoveVRP2Opt<ESolutionHFMVRP> super;

 private:
  ProblemInstance* hfmvrp;

  using Routes = std::vector<vector<int>>;

 public:
  DeltaMoveVRP2Opt(std::function<Routes&(const ESolutionHFMVRP&)> _getRoutes,
                   int _r, int _p1, int _p2, ProblemInstance* _hfmvrp)
      : super(getRoutes, _r, _p1, _p2), hfmvrp(_hfmvrp) {
    if (!_hfmvrp) {
      std::cout << "Error: hfmvrp problem is NULL!" << std::endl;
      print();
      exit(1);
    }
  }

  /*
          DeltaMoveVRP2Opt(int _r, int _p1, int _p2, ProblemInstance* _hfmvrp) :
                  super(localGetRoutes, _r, _p1, _p2), hfmvrp(_hfmvrp)
          {
                  if (!_hfmvrp)
                  {
                          std::cout << "Error: hfmvrp problem is NULL!" << std::endl;
                          print();
                          exit(1);
                  }
          }
  */

  virtual ~DeltaMoveVRP2Opt() {}

  int myabs(int x) { return std::abs(x); }

  virtual bool canBeApplied(const ESolutionHFMVRP& se) {
    const SolutionHFMVRP& s = se.first;
    const RepHFMVRP& rep = s.getR();
    // const AdsHFMVRP& ads = s.getADS();

    int& r = this->r;
    int& p1 = this->p1;
    int& p2 = this->p2;

    if (r >= 0) {
      bool all_positive = (p1 >= 1) && p1 < (((int)rep[r].size()) - 1) &&
                          (p2 >= 1) && p2 < (((int)rep[r].size()) - 1);
      bool minSize = (rep[r].size() >= 3);

      // std::cout << "VRP 2 OPT canBeApplied 1 " << std::endl;
      // std::cout << "idComponent() = " << idComponent() << std::endl;
      // std::cout << "VRP 2 OPT upNeigh 1" << std::endl;

      // std::cout << "ads.neighborhoodStatus.size() = " <<
      // ads.neighborhoodStatus.size() << std::endl;
      ////cout << "r = " << r << std::endl;
      // std::cout << "ads.neighborhoodStatus.find(idComponent())->second)[r] = " <<
      // (ads.neighborhoodStatus.find(idComponent())->second)[r] << std::endl; std::cout
      // << "VRP 2 OPT upNeigh 2" << std::endl;

      // getchar();
      // std::cout << "VRP 2 OPT canBeApplied 2 " << std::endl;
      // getchar();
      return all_positive && minSize;
    } else {
      return 0;
    }
  }

  void updateNeighStatus(AdsHFMVRP& ads) {
    for (map<string, vector<bool>>::iterator iter =
             ads.neighborhoodStatus.begin();
         iter != ads.neighborhoodStatus.end(); iter++) {
      ads.neighborhoodStatus[iter->first][r] = false;
    }
  }

  void updateModifiedRoutes(RepHFMVRP& rep, AdsHFMVRP& ads) {
    vector<int> modRoutes;  // modified routes
    modRoutes.push_back(r);

    for (int iter = 0; iter < ((int)modRoutes.size()); iter++) {
      int r = modRoutes[iter];
      vector<int> route = rep[r];
      int routeSize = route.size();

      vector<double> routeDemands(routeSize);

      routeDemands[0] = 0;              // depot demand
      routeDemands[routeSize - 1] = 0;  // depot demand

      ads.cumulativeDemand[r].clear();  // TODO leak memoria
      ads.cumulativeDemand[r].resize(routeSize);

      ads.cumulativeDemand[r][0] = 0;              // depot cumulative demand
      ads.cumulativeDemand[r][routeSize - 1] = 0;  // depot cumulative demand

      for (int c = 1; c < int(routeSize - 1); c++) {
        routeDemands[c] = hfmvrp->demands[route[c]];
        ads.cumulativeDemand[r][c] =
            routeDemands[c] +
            ads.cumulativeDemand[r][c - 1];  // calculate the cumulative demand
                                             // for it s of the route
      }

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
        minPairDemand = routeDemands[1];
        maxPairDemand = routeDemands[1];
      }

      ads.maxPairDemand[r] = maxPairDemand;
      ads.minPairDemand[r] = minPairDemand;
    }
  }

  uptr<Move<ESolutionHFMVRP>> apply(ESolutionHFMVRP& se) override {
    SolutionHFMVRP& s = se.first;
    RepHFMVRP& rep = s.getR();
    AdsHFMVRP& ads = s.getADS();

    int small, bigger;
    if (p1 <= p2) {
      small = p1;
      bigger = p2;
    } else {
      small = p2;
      bigger = p1;
    }

    reverse(rep.at(r).begin() + small, rep.at(r).begin() + bigger);

    // Update ADS
    updateNeighStatus(ads);

    // Update minDemand,maxDemand, minPairDemand, maxPairDemand, cumulative and
    // sum
    updateModifiedRoutes(rep, ads);

    return uptr<Move<ESolutionHFMVRP>>(
        new DeltaMoveVRP2Opt(super::getRoutes, r, p1, p2, hfmvrp));
  }

  op<EvaluationHFMVRP> cost(const pair<SolutionHFMVRP, Evaluation<>>& se,
                            bool allowEstimated) override {
    const SolutionHFMVRP& s = se.first;
    const RepHFMVRP& rep = s.getR();
    vector<int> route = rep[r];
    int vType = -1;
    if (r >= hfmvrp->nVehicles)
      vType = 2;  // MultiTripVehicle
    else
      vType = hfmvrp->getVehicleType(r);

    double vTDC = hfmvrp->getVehicleTypeDistCost(vType);

    unsigned k1 = p1;
    unsigned k2 = p2;

    // before k1 and k2
    unsigned bk1 = k1 - 1;
    unsigned bk2 = k2 - 1;

    if (k1 == 0) bk1 = route.size() - 1;
    if (k2 == 0) bk2 = route.size() - 1;

    double f = 0;

    if (myabs(k1 - k2) >= 2) {
      f -= hfmvrp->getDist(route[bk1], route[k1]) * vTDC;
      // std::cout << "-d(" << rep[bk1] << "," << rep[k1] << ") ";
      f -= hfmvrp->getDist(route[bk2], route[k2]) * vTDC;
      // std::cout << "-d(" << rep[bk2] << "," << rep[k2] << ") ";

      f += hfmvrp->getDist(route[bk1], route[bk2]) * vTDC;
      // std::cout << "+d(" << rep[bk1] << "," << rep[bk2] << ") ";
      f += hfmvrp->getDist(route[k1], route[k2]) * vTDC;
      // std::cout << "+d(" << rep[k1] << "," << rep[k2] << ") ";
    }
    // std::cout << std::endl;

    /// return new MoveCost<> (f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  static std::string idComponent() {
    string idComp = super::idComponent();
    idComp.append(":DeltaMoveVRP2Opt");
    return idComp;
  }

  virtual bool operator==(const Move<ESolutionHFMVRP>& _m) const {
    const DeltaMoveVRP2Opt& m1 = (const DeltaMoveVRP2Opt&)_m;
    return ((m1.p1 == p1) && (m1.p2 == p2));
  }

  void print() const override {
    std::cout << "DeltaMoveVRP2Opt( route: " << r << " : ";
    std::cout << " edge " << p1 << " <=>  edge " << p2 << " )";
    std::cout << std::endl;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRP2OPT_HPP_
