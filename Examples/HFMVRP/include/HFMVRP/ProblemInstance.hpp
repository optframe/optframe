// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef HFMVRP_PROBLEMINSTANCE_HPP_
#define HFMVRP_PROBLEMINSTANCE_HPP_

// C++
#include <iostream>
#include <utility>

//
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace HFMVRP {
static bool compare(pair<int, double> p1, pair<int, double> p2) {
  return p1.second > p2.second;
}

class ProblemInstance {
 private:
  // Your private vars

  template <typename T>
  void printv(const std::vector<T>& v) {
    std::cout << "vector(" << v.size() << "): ";
    for (auto& i : v) std::cout << i << " ";
    std::cout << std::endl;
  }

 public:
  explicit ProblemInstance(scannerpp::Scanner& scanner) {
    for (int i = 0; i < 4; i++) std::cout << scanner.nextLine() << std::endl;
    // Problem data
    std::cout << scanner.next() << " ";
    nodes = *scanner.nextDouble();
    std::cout << nodes << std::endl;

    std::cout << scanner.next() << " ";
    nVehicles = *scanner.nextDouble();
    std::cout << nVehicles << std::endl;

    std::cout << scanner.next() << " ";
    typesVehicles = *scanner.nextDouble();
    std::cout << typesVehicles << std::endl;

    // Veihicles types
    for (int i = 0; i < 3; i++) std::cout << scanner.nextLine() << std::endl;
    double sumCap = 0;
    for (int i = 0; i < typesVehicles; i++) {
      std::cout << scanner.next() << " ";  // id
      vehiclesTypeNumber.push_back((int)*scanner.nextDouble());
      vehiclesTypeCap.push_back((int)*scanner.nextDouble());
      vehiclesTypeDistCost.push_back(*scanner.nextDouble());
      vehiclesTypeFixedCost1.push_back(*scanner.nextDouble());
      vehiclesTypeFixedBoxCost2.push_back(*scanner.nextDouble());
      theoricalCap = 22;
      vehiclesTypeFixedCost.push_back(vehiclesTypeFixedCost1[i] +
                                      (vehiclesTypeCap[i] - theoricalCap) *
                                          vehiclesTypeFixedBoxCost2[i]);
      std::cout << vehiclesTypeNumber[i] << " " << vehiclesTypeCap[i] << " "
           << vehiclesTypeFixedCost[i] << " " << vehiclesTypeDistCost[i]
           << std::endl;
      sumCap += vehiclesTypeCap[i] * vehiclesTypeNumber[i];
    }

    // Stores IDS
    for (int i = 0; i < 3; i++) std::cout << scanner.nextLine() << std::endl;

    for (int i = 0; i < nodes; i++) {
      int storeID = *scanner.nextDouble();
      storeIDS.push_back(storeID);
    }

    // Matrix of Distances
    for (int i = 0; i < 3; i++) std::cout << scanner.nextLine() << std::endl;

    dist = new optframe::Matrix<double>(nodes);

    int nullDist = 0;
    for (int i = 0; i < nodes; i++) {
      for (int j = 0; j < nodes; j++) {
        // std::cout<<"i= "<<i<<"j= "<<j<<endl;
        double nodeDist = *scanner.nextDouble();
        // std::cout<<"nodeDist = "<<nodeDist<<endl;
        // getchar();
        if (nodeDist == 999) {
          nodeDist = 1000000;
          nullDist++;
        }
        (*dist)(i, j) = nodeDist;
      }
    }

    // std::cout<<"nullDist = "<<nullDist<<endl;
    // getchar();

    //		cout << "dist: " << (*dist);

    // Demands
    double sumDemand = 0;
    for (int i = 0; i < 3; i++) std::cout << scanner.nextLine() << std::endl;

    int nullDemands = 0;
    for (int c = 0; c < nodes; c++) {
      double demand = *scanner.nextDouble();
      demands.push_back(demand);
      if (demands[c] == 0) nullDemands++;
      sumDemand += demand;
    }

    // std::cout << demands << std::endl;
    printv(demands);

    std::cout << "active demands =" << demands.size() - nullDemands << std::endl;

    // Comp
    for (int i = 0; i < 3; i++) std::cout << scanner.nextLine() << std::endl;

    comp = new Matrix<double>(typesVehicles, nodes);

    for (int c = 0; c < nodes; c++) {
      int nComp = 0;
      for (int t = 0; t < typesVehicles; t++) {
        // std::cout<<"i= "<<i<<"j= "<<j<<endl;
        double compTruck = *scanner.nextDouble();
        // std::cout<<"nodeDist = "<<nodeDist<<endl;
        // getchar();
        (*comp)(t, c) = compTruck;
        if (compTruck == true) nComp++;
      }
      nStoreComp.push_back(nComp);
    }

    std::cout << "comp: " << (*comp);

    // creat list of vehicles cap and costs
    for (int v = 0; v < nVehicles; v++) {
      int vType = getVehicleType(v);
      vehiclesCap.push_back(getVehicleTypeCap(vType));
      vehiclesFixedCost.push_back(getVehicleTypeFixedCost(vType));
      vehiclesDistCost.push_back(getVehicleTypeDistCost(vType));
    }

    /*
     for (int v = 0; v < nVehicles; v++)
     {
     std::cout << v << "\t" << vehiclesCap[v] << "\t" << vehiclesFixedCost[v] <<
     "\t"
     << vehiclesDistCost[v] << std::endl;
     }*/

    std::cout << "Sum Cap = " << sumCap << "\tSum Demand = " << sumDemand << std::endl;
    if (sumCap < sumDemand) std::cout << "MULTI-TRIP INSTANCE" << std::endl;
    std::cout << "#########################################################" << std::endl;
    std::cout << "############## Problem Instance Read ####################" << std::endl;
    std::cout << "#########################################################" << std::endl;
  }

  virtual ~ProblemInstance() {}

  int getVehicleType(int v) {
    int type = 0;
    int sumTypes = vehiclesTypeNumber[type];
    while (sumTypes <= v) {
      type++;
      sumTypes += vehiclesTypeNumber[type];
    }

    return type;
  }

  int getVehicleID(int type) {
    int id = 0;
    int sumTypes = 0;
    while (sumTypes != type) {
      id += vehiclesTypeNumber[sumTypes];
      sumTypes++;
    }

    return id;
  }

  double getVehicleCap(int v) { return vehiclesCap[v]; }

  double getVehicleDistCost(int v) { return vehiclesDistCost[v]; }

  double getVehicleFixedCost(int v) { return vehiclesFixedCost[v]; }

  double getVehicleTypeCap(int v) { return vehiclesTypeCap[v]; }

  double getVehicleTypeDistCost(int v) { return vehiclesTypeDistCost[v]; }

  double getVehicleTypeFixedCost(int v) { return vehiclesTypeFixedCost[v]; }

  double getVehicleNumber(int v) { return vehiclesTypeNumber[v]; }

  double getDist(int i, int j) { return (*dist)(i, j); }

  int getStoreID(int s) { return storeIDS[s]; }

  bool getComp(int v, int c) { return (*comp)(v, c); }

  double nodes;
  double nVehicles;
  double typesVehicles;
  // Matrizes
  Matrix<double>* dist;
  Matrix<double>* comp;
  vector<int> nStoreComp;
  // vectors
  vector<int> storeIDS;
  vector<double> demands;
  vector<int> vehiclesTypeNumber;
  vector<int> vehiclesTypeCap;
  vector<double> vehiclesTypeFixedCost;
  vector<double> vehiclesTypeFixedBoxCost2;
  vector<double> vehiclesTypeFixedCost1;
  vector<double> vehiclesTypeDistCost;
  vector<int> vehiclesCap;
  vector<double> vehiclesFixedCost;
  vector<double> vehiclesDistCost;
  int theoricalCap;
};

}  // namespace HFMVRP

#endif /*HFMVRP_PROBLEMINSTANCE_HPP_*/
