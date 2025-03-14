// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef MODM_ADSMANAGER_H_
#define MODM_ADSMANAGER_H_

#include <OptFrame/Helper/ADSManager.hpp>

#include "ADS.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace MODM {

class MODMADSManager : public ADSManager<RepMODM, AdsMODM, SolutionMODM> {
 private:
  typedef ADSManager<RepMODM, AdsMODM, SolutionMODM> super;
  ProblemInstance& pMODM;
  int c, n;

 public:
  MODMADSManager(ProblemInstance& _pMODM) : pMODM(_pMODM) {
    c = pMODM.getNumberOfClients();
    n = pMODM.getNumberOfProducts();
    Component::verbose = false;
  }

  virtual ~MODMADSManager() {}

  // initialize ADS as you wish
  virtual void initializeADSNeighStructure(const RepMODM& rep, AdsMODM& ads) {}

  // update ADS as you wish
  virtual void setNeighLocalOptimum(const RepMODM& rep, AdsMODM& ads,
                                    string moveID) {
    for (int r = 0; r < rep.size(); r++)
      ads.neighborhoodStatus[moveID][r] = true;
  }

  void initializeADS(const RepMODM& newRep, AdsMODM& newAds) {
    newAds.clientOffers.clear();
    newAds.productOffers.clear();
    newAds.totalCost.clear();
    newAds.totalRevenue.clear();

    newAds.clientOffers.resize(c);
    newAds.productOffers.resize(n);
    newAds.totalCost.resize(n);
    newAds.totalRevenue.resize(n);

    vector<int> _clientOffers(c);
    vector<int> _productOffers(n);
    vector<int> _totalCost(n);
    vector<int> _totalRevenue(n);

    for (int product = 0; product < n; product++)
      for (int client = 0; client < c; client++) {
        _totalCost[product] +=
            (newRep[client][product] * pMODM.getCost(client, product));
        _totalRevenue[product] +=
            (newRep[client][product] * pMODM.getRevenue(client, product));
        _productOffers[product] += newRep[client][product];
        _clientOffers[client] += newRep[client][product];
      }

    newAds.clientOffers = _clientOffers;
    newAds.productOffers = _productOffers;
    newAds.totalCost = _totalCost;
    newAds.totalRevenue = _totalRevenue;

    if (Component::verbose) {
      std::cout << "Prints Initialize ADS" << std::endl;
      std::cout << "newAds.clientOffers:";
      std::cout << newAds.clientOffers << std::endl;
      std::cout << "newAds.productOffers:";
      std::cout << newAds.productOffers << std::endl;
      std::cout << "newAds.totalCost:";
      std::cout << newAds.totalCost << std::endl;
      std::cout << "newAds.totalRevenue:";
      std::cout << newAds.totalRevenue << std::endl << std::endl;
    }
  }

  virtual bool compareADS(const AdsMODM& _ads1, const AdsMODM& _ads2) {
    if (_ads1.clientOffers != _ads1.clientOffers) {
      std::cout << "Error on Client Offers Vector!" << std::endl;
      // getchar();
      return false;
    }

    if (_ads1.productOffers != _ads1.productOffers) {
      std::cout << "Error on productOffers Vector!" << std::endl;
      // getchar();
      return false;
    }

    if (_ads1.totalCost != _ads1.totalCost) {
      std::cout << "Error on totalCost Vector!" << std::endl;
      // getchar();
      return false;
    }

    if (_ads1.totalRevenue != _ads1.totalRevenue) {
      std::cout << "Error on totalRevenue Vector!" << std::endl;
      // getchar();
      return false;
    }
    return true;
  }

  virtual void printADS(const AdsMODM& ads) {
    std::cout << "---------------------------------" << std::endl;
    std::cout << "ADS print:" << std::endl;
    std::cout << ads.clientOffers << std::endl;
    std::cout << ads.productOffers << std::endl;
    std::cout << ads.totalCost << std::endl;
    std::cout << ads.totalRevenue << std::endl;
    std::cout << "FINISH ADS print" << std::endl;
    std::cout << "---------------------------------" << std::endl;
  }
};

}  // namespace MODM
#endif /* MODM_ADSMANAGER_H_ */
