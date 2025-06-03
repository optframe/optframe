// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRPOROPT1_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRPOROPT1_HPP_

#include <cmath>
//
#include <OptFrame/Util/NSAdapter/VRP/Intra/NSSeqVRPOrOpt1.hpp>

using namespace std;

namespace HFMVRP {

class DeltaMoveVRPOrOpt1
    : public MoveVRPOrOpt1<ESolutionHFMVRP>  //<int, AdsHFMVRP, SolutionHFMVRP>
{
  typedef MoveVRPOrOpt1<ESolutionHFMVRP> super;

 private:
  ProblemInstance* hfmvrp;
  int k;

  using Routes = std::vector<vector<int>>;

 public:
  DeltaMoveVRPOrOpt1(std::function<Routes&(const ESolutionHFMVRP&)> _getRoutes,
                     int _r, int _c, int _pos, ProblemInstance* _hfmvrp)
      : super(_getRoutes, _r, _c, _pos), hfmvrp(_hfmvrp) {
    k = 1;  // OrOpt1
    if (!_hfmvrp) {
      std::cout << "Error: hfmvrp problem is NULL!" << std::endl;
      print();
      exit(1);
    }
  }

  virtual ~DeltaMoveVRPOrOpt1() {}

  int myabs(int x) { return std::abs(x); }

  bool canBeApplied(const ESolutionHFMVRP& se) {
    const SolutionHFMVRP& s = se.first;
    const RepHFMVRP& rep = s.getR();
    if (r >= 0) {
      bool all_positive = (r >= 0) && (c >= 1) &&
                          (c < (((int)rep[r].size()) - 1)) && (pos >= 1) &&
                          (pos < (((int)rep[r].size()) - 1));
      bool minSize = (((int)rep[r].size()) >= 3);

      return all_positive && minSize && (c != pos) && (c + 1 != pos) &&
             ((c - pos) >= k);
    } else {
      return 0;
    }
  }

  virtual void updateNeighStatus(AdsHFMVRP& ads) {
    for (map<string, vector<bool>>::iterator iter =
             ads.neighborhoodStatus.begin();
         iter != ads.neighborhoodStatus.end(); iter++) {
      //			cout<<r<<"\titer->first: "<<iter->first<<endl;
      ads.neighborhoodStatus.at(iter->first).at(r) = false;
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
    // Update NeightStatus
    //		print();
    updateNeighStatus(ads);

    int aux;
    int i = 0;
    if (c < pos) {
      aux = rep.at(r).at(c);
      for (i = c; i < (pos - 1); i++) {
        // aux = rep.at(r).at(i);
        rep.at(r).at(i) = rep.at(r).at(i + 1);
        // rep.at(r).at(i + 1) = aux;
      }
      rep.at(r).at(i) = aux;

      // Update ADS
      // Update minDemand,maxDemand, minPairDemand, maxPairDemand, cumulative
      // and sum
      updateModifiedRoutes(rep, ads);

      return uptr<Move<ESolutionHFMVRP>>(
          new DeltaMoveVRPOrOpt1(super::getRoutes, r, pos - 1, c, hfmvrp));
    } else {
      aux = rep.at(r).at(c);
      for (i = c; i > pos; i--) {
        rep.at(r).at(i) = rep.at(r).at(i - 1);
      }
      rep.at(r).at(i) = aux;

      // Update ADS
      // Update minDemand,maxDemand, minPairDemand, maxPairDemand, cumulative
      // and sum
      updateModifiedRoutes(rep, ads);

      return uptr<Move<ESolutionHFMVRP>>(
          new DeltaMoveVRPOrOpt1(super::getRoutes, r, pos, c + 1, hfmvrp));
    }
  }

  op<EvaluationHFMVRP> cost(const pair<SolutionHFMVRP, Evaluation<>>& se,
                            bool allowEstimated) override {
    const SolutionHFMVRP& s = se.first;
    const RepHFMVRP& rep = s.getR();
    // const AdsHFMVRP& ads = s.getADS();
    vector<int> route = rep[r];
    int routeSize = route.size();
    // std::cout << "routeSize = " << routeSize << std::endl;
    // std::cout << route << std::endl;

    int vType = -1;
    if (r >= hfmvrp->nVehicles)
      vType = 2;  // MultiTripVehicle
    else
      vType = hfmvrp->getVehicleType(r);

    double vTDC = hfmvrp->getVehicleTypeDistCost(vType);

    // before c and pos
    int bi = c - 1;
    int bj = pos - 1;

    if (bi < 0) {
      std::cout << "BUGGGGGG====================" << std::endl;
      getchar();
      bi = routeSize - 2;
    }
    if (bj < 0) {
      std::cout << "BUGGGGGG====================" << std::endl;
      getchar();
      bj = routeSize - 2;
    }

    // after j
    unsigned aj = pos + 1;

    if (aj == (unsigned)routeSize)  // depot at the for each route
    {
      aj = 1;
      std::cout << "BUGGGGGG====================" << std::endl;
      getchar();
    }

    // last i+k
    unsigned lik = c + k - 1;

    // after i+k
    unsigned aik = c + k;

    if (aik == (unsigned)routeSize) {
      aik = 1;
      std::cout << "BUGGGGGG====================" << std::endl;
      getchar();
    }

    double f = 0;

    if (((c == routeSize - 2) && (pos == 0)) ||
        ((c == 0) && (pos == routeSize - 2)))  // extreme point
    {
      f = 0;
    } else {
      if (c < pos) {
        f -= hfmvrp->getDist(route[bi], route[c]) * vTDC;
        // std::cout << "-d(" << rep[r][bi] << "," << rep[r][c] << ") \t f= " << f <<
        // std::endl;
        f -= hfmvrp->getDist(route[lik], route[aik]) * vTDC;
        // std::cout << "-d(" << rep[r][lik] << "," << rep[r][aik] << ") \t f= " << f
        // << std::endl;
        f -= hfmvrp->getDist(route[pos], route[aj]) * vTDC;
        // std::cout << "-d(" << rep[r][pos] << "," << rep[r][aj] << ") \t f= " << f
        // << std::endl;

        f += hfmvrp->getDist(route[bi], route[aik]) * vTDC;
        // std::cout << "+d(" << rep[r][bi] << "," << rep[r][aik] << ") \t f= " << f
        // << std::endl;
        f += hfmvrp->getDist(route[pos], route[c]) * vTDC;
        // std::cout << "+d(" << rep[r][pos] << "," << rep[r][c] << ") \t f= " << f
        // << std::endl;
        f += hfmvrp->getDist(route[lik], route[aj]) * vTDC;
        // std::cout << "+d(" << rep[r][lik] << "," << rep[r][aj] << ") \t f= " << f
        // << std::endl;
      } else  // c > pos
      {
        f -= hfmvrp->getDist(route[bi], route[c]) * vTDC;
        // std::cout << "-d(" << rep[r][bi] << "," << rep[r][c] << ") \t f= " << f <<
        // std::endl;
        f -= hfmvrp->getDist(route[lik], route[aik]) * vTDC;
        // std::cout << "-d(" << rep[r][lik] << "," << rep[r][aik] << ") \t f= " << f
        // << std::endl;
        f -= hfmvrp->getDist(route[bj], route[pos]) * vTDC;
        // std::cout << "-d(" << rep[r][bj] << "," << rep[r][pos] << ") \t f= " << f
        // << std::endl;

        f += hfmvrp->getDist(route[bi], route[aik]) * vTDC;
        // std::cout << "+d(" << rep[r][bi] << "," << rep[r][aik] << ") \t f= " << f
        // << std::endl;
        f += hfmvrp->getDist(route[bj], route[c]) * vTDC;
        // std::cout << "+d(" << rep[r][bj] << "," << rep[r][c] << ") \t f= " << f <<
        // std::endl;
        f += hfmvrp->getDist(route[lik], route[pos]) * vTDC;
        // std::cout << "+d(" << rep[r][lik] << "," << rep[r][pos] << ") \t f= " << f
        // << std::endl;
      }
    }

    /// return new MoveCost<> (f, 0);
    return make_optional(Evaluation<>(f, 0));
  }

  std::string id() const override {
    stringstream ssIdRoute;
    ssIdRoute << super::idComponent() << ":DeltaMoveVRPOrOpt1"
              << "+Route=" << r;
    return ssIdRoute.str();
  }

  static std::string idComponent() {
    string idComp = super::idComponent();
    idComp.append(":DeltaMoveVRPOrOpt1");
    return idComp;
  }

  virtual bool operator==(const Move<ESolutionHFMVRP>& _m) const {
    const DeltaMoveVRPOrOpt1& m1 = (const DeltaMoveVRPOrOpt1&)_m;
    return (m1.r == r) && (m1.c == c) && (m1.pos == pos);
  }

  void print() const override {
    std::cout << "DeltaMoveVRPOrOpt1( route: " << r << " , " << c << " , " << pos
         << " )" << std::endl;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAMOVEVRPOROPT1_HPP_
