// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_ADSMANAGERHFMVRP_HPP_
#define OPTFRAME_ADSMANAGERHFMVRP_HPP_

using namespace std;

namespace HFMVRP {

class HFMVRPADSManager
    : public ADSManager<RepHFMVRP, AdsHFMVRP, SolutionHFMVRP> {
  typedef ADSManager<RepHFMVRP, AdsHFMVRP, SolutionHFMVRP> super;
  ProblemInstance& hfmvrp;

 public:
  HFMVRPADSManager(ProblemInstance& _hfmvrp) : hfmvrp(_hfmvrp) {}

  virtual ~HFMVRPADSManager() {}

  // initialize ADS as you wish
  virtual void initializeADSNeighStructure(const RepHFMVRP& rep,
                                           AdsHFMVRP& ads) {
    /*
     std::cout << "Entrou!" << std::endl;
     getchar();
     for (map<string, vector<bool> >::iterator iter =
     newAds.neighborhoodStatus.begin(); iter != newAds.neighborhoodStatus.end();
     iter++)
     {
     std::cout << "iter->first = " << iter->first << std::endl;
     std::cout << "iter->second = " << iter->second << std::endl;
     }

     getchar();
     */

    for (int r = 0; r < (int)rep.size(); r++) {
      (ads.neighborhoodStatus[DeltaMoveVRP2Opt::idComponent()])
          .push_back(false);
      (ads.neighborhoodStatus[DeltaMoveVRPOrOpt1::idComponent()])
          .push_back(false);
      (ads.neighborhoodStatus[DeltaMoveVRPOrOpt2::idComponent()])
          .push_back(false);
      (ads.neighborhoodStatus[DeltaMoveVRPExchange::idComponent()])
          .push_back(false);
      (ads.neighborhoodStatus[DeltaMoveVRPSwap1_1::idComponent()])
          .push_back(false);
      (ads.neighborhoodStatus[DeltaMoveVRPShift10::idComponent()])
          .push_back(false);
    }
    /*

     for (map<string, vector<bool> >::iterator iter =
     ads.neighborhoodStatus.begin(); iter != ads.neighborhoodStatus.end();
     iter++)
     {
     std::cout << "iter->first = " << iter->first << std::endl;
     std::cout << "iter->second = " << iter->second << std::endl;
     }
     std::cout << "Finalizou!" << std::endl;

     getchar();
     */
  }

  // update ADS as you wish
  virtual void setNeighLocalOptimum(const RepHFMVRP& rep, AdsHFMVRP& ads,
                                    string moveID) {
    // std::cout << "moveID= " << moveID << std::endl;
    // getchar();
    for (int r = 0; r < (int)rep.size(); r++)
      ads.neighborhoodStatus[moveID][r] = true;
  }

  void initializeADS(const RepHFMVRP& newRep, AdsHFMVRP& newAds) {
    int nRoutes = newRep.size();  // nVehicles plus multi-trip Vehicles
    newAds.sumDemand.clear();
    newAds.minDemand.clear();
    newAds.maxDemand.clear();
    newAds.minPairDemand.clear();
    newAds.maxPairDemand.clear();
    newAds.cumulativeDemand.clear();

    newAds.sumDemand.resize(nRoutes);
    newAds.minDemand.resize(nRoutes);
    newAds.maxDemand.resize(nRoutes);
    newAds.minPairDemand.resize(nRoutes);
    newAds.maxPairDemand.resize(nRoutes);
    newAds.cumulativeDemand.resize(nRoutes);

    for (int r = 0; r < nRoutes; r++) {
      vector<int> route = newRep[r];
      int routeSize = route.size();
      if (routeSize >= 3)  // two depots plus one client
      {
        vector<double> routeDemands(routeSize);

        routeDemands[0] = 0;              // depot demand
        routeDemands[routeSize - 1] = 0;  // depot demand
        vector<double> cDemand;
        cDemand.push_back(0);

        for (int s = 1; s < int(routeSize - 1); s++) {
          routeDemands[s] = hfmvrp.demands[route[s]];
          newAds.sumDemand[r] += routeDemands[s];  // increment the total demand
          cDemand.push_back(routeDemands[s] +
                            cDemand[s - 1]);  // calculate the cumulative demand
                                              // for it s of the route
        }

        cDemand.push_back(0);

        newAds.cumulativeDemand[r] =
            cDemand;  // set  the cumulative demand to the vehicle

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

        newAds.maxPairDemand[r] = maxPairDemand;
        newAds.minPairDemand[r] = minPairDemand;

        // minDemand and maxDemand Calculus
        std::vector<std::pair<int, double> > rankDemands;
        for (int i = 1; i < int(routeSize - 1); i++)
          rankDemands.push_back(std::make_pair(i, routeDemands[i]));

        // std::cout << rankDemands << std::endl;

        sort(rankDemands.begin(), rankDemands.end(), compare);

        newAds.maxDemand[r] = rankDemands[0].second;
        newAds.minDemand[r] = rankDemands[rankDemands.size() - 1].second;
      }
    }

    /*
     std::cout << "Initialized ADS" << std::endl;
     std::cout << "newAds.cumulativeDemand" << newAds.cumulativeDemand << std::endl;
     std::cout << "newAds.maxDemand" << newAds.maxDemand << std::endl;
     std::cout << "newAds.minDemand" << newAds.minDemand << std::endl;
     std::cout << "newAds.maxPairDemand" << newAds.maxPairDemand << std::endl;
     std::cout << "newAds.minPairDemand" << newAds.minPairDemand << std::endl;
     std::cout << "newAds.sumDemand" << newAds.sumDemand << std::endl;

     getchar();
     */
  }

  virtual bool compareADS(const AdsHFMVRP& _ads1, const AdsHFMVRP& _ads2) {
    bool compare = true;
    if (_ads1.maxDemand != _ads2.maxDemand) {
      std::cout << "Error on maxDemand vector!" << std::endl;
      std::cout << _ads1.maxDemand << std::endl;
      std::cout << _ads2.maxDemand << std::endl;
      for (int r = 0; r < (int)_ads1.maxDemand.size(); r++) {
        if (_ads1.maxDemand[r] != _ads2.maxDemand[r]) {
          std::cout << "r = " << r << std::endl;
          std::cout << _ads1.maxDemand[r] << std::endl;
          std::cout << _ads2.maxDemand[r] << std::endl;
          getchar();
        }
      }

      return false;
    }
    if (_ads1.minDemand != _ads2.minDemand) {
      std::cout << "Error on minDemand vector!" << std::endl;
      std::cout << _ads1.minDemand << std::endl;
      std::cout << _ads2.minDemand << std::endl;
      for (int r = 0; r < (int)_ads1.minDemand.size(); r++) {
        if (_ads1.minDemand[r] != _ads2.minDemand[r]) {
          std::cout << "r = " << r << std::endl;
          std::cout << _ads1.minDemand[r] << std::endl;
          std::cout << _ads2.minDemand[r] << std::endl;
          getchar();
        }
      }
      return false;
    }
    if (_ads1.maxPairDemand != _ads2.maxPairDemand) {
      std::cout << "Error on maxPairDemand vector!" << std::endl;
      std::cout << _ads1.maxPairDemand << std::endl;
      std::cout << _ads2.maxPairDemand << std::endl;
      for (int r = 0; r < (int)_ads1.maxPairDemand.size(); r++) {
        if (_ads1.maxPairDemand[r] != _ads2.maxPairDemand[r]) {
          std::cout << "r = " << r << std::endl;
          std::cout << _ads1.maxPairDemand[r] << std::endl;
          std::cout << _ads2.maxPairDemand[r] << std::endl;
          getchar();
        }
      }
      return false;
    }
    if (_ads1.minPairDemand != _ads2.minPairDemand) {
      std::cout << "Error on minPairDemand vector!" << std::endl;
      std::cout << _ads1.minPairDemand << std::endl;
      std::cout << _ads2.minPairDemand << std::endl;
      for (int r = 0; r < (int)_ads1.minPairDemand.size(); r++) {
        if (_ads1.minPairDemand[r] != _ads2.minPairDemand[r]) {
          std::cout << "r = " << r << std::endl;
          std::cout << _ads1.minPairDemand[r] << std::endl;
          std::cout << _ads2.minPairDemand[r] << std::endl;
          getchar();
        }
      }
      return false;
    }
    if (_ads1.sumDemand != _ads2.sumDemand) {
      std::cout << "Error on sumDemand vector!" << std::endl;
      return false;
    }

    if (_ads1.cumulativeDemand != _ads2.cumulativeDemand) {
      std::cout << _ads1.cumulativeDemand << std::endl;
      std::cout << _ads2.cumulativeDemand << std::endl;
      std::cout << "Error on CumulativeDemand vector!" << std::endl;
      for (int r = 0; r < (int)_ads1.cumulativeDemand.size(); r++) {
        for (int c = 0; c < (int)_ads1.cumulativeDemand[r].size(); c++) {
          if (_ads1.cumulativeDemand[r][c] != _ads2.cumulativeDemand[r][c]) {
            std::cout << "r = " << r << "\tc = " << c << std::endl;
            std::cout << _ads1.cumulativeDemand[r][c] << std::endl;
            std::cout << _ads2.cumulativeDemand[r][c] << std::endl;
            getchar();
          }
        }
      }
      return false;
    }
    return compare;
  }

  virtual void printADS(const AdsHFMVRP& ads) {
    std::cout << "ads.cumulativeDemand" << ads.cumulativeDemand << std::endl;
    std::cout << "ads.maxDemand" << ads.maxDemand << std::endl;
    std::cout << "ads.minDemand" << ads.minDemand << std::endl;
    std::cout << "ads.maxPairDemand" << ads.maxPairDemand << std::endl;
    std::cout << "ads.minPairDemand" << ads.minPairDemand << std::endl;
    std::cout << "ads.sumDemand" << ads.sumDemand << std::endl;
  }
};
}  // namespace HFMVRP
#endif /* OPTFRAME_UPDATEADSHFMVRP_HPP_ */
