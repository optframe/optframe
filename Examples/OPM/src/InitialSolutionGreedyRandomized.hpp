#ifndef OPM_INITIALSOLUTION_GreedyRandomized_HPP_
#define OPM_INITIALSOLUTION_GreedyRandomized_HPP_

#include <OptFrame/Core/Constructive.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp"
#include <stdlib.h>

#include <OptFrame/Core/RandGen.hpp>
#include <algorithm>
#include <list>

#include "Evaluator.hpp"
#include "ProblemInstance.hpp"
#include "Representation.h"
#include "Solution.h"

using namespace std;

typedef pair<int, double> my_pair;

// sort predicate
bool sort_pred(const my_pair& left, const my_pair& right) {
  return left.second < right.second;
}

// absolute value
double absval(double value) {
  if (value < 0)
    return value * (-1);
  else
    return value;
}

namespace POLAD {

class OPMInitialSolutionGreedyRandomized : public Constructive<RepOPM> {
 private:
  OPMProblemInstance& opm;
  RandGen& rg;

 public:
  OPMInitialSolutionGreedyRandomized(OPMProblemInstance& _opm, RandGen& _rg)
      : opm(_opm), rg(_rg) {}

  SolutionOPM* generateSolution(double timelimit) override {
    double alpha_w = rg.rand01();  // for waste rock pits
    double alpha_o = rg.rand01();  // for ore pits
    double alpha_s = rg.rand01();  // for shovels
    if (alpha_w == 0) alpha_w = 0.001;
    if (alpha_o == 0) alpha_o = 0.001;
    if (alpha_s == 0) alpha_s = 0.001;

    return generateSolution(alpha_w, alpha_o, alpha_s);
  }

  SolutionOPM* generateSolution(double alpha_w, double alpha_o,
                                double alpha_s) {
    // Create new vector for shovels
    vector<int> vs(opm.numFrentes, -1);

    // Create new matrix for trips
    Matrix<int> trips(opm.numFrentes, opm.numCaminhoes);
    trips.fill(0);

    generateWasteRockSolution(&vs, &trips, alpha_w, alpha_s);
    generateOreSolution(&vs, &trips, alpha_o, alpha_s);

    return new SolutionOPM(*new pair<vector<int>, Matrix<int> >(vs, trips));
  }

  void generateWasteRockSolution(std::vector<int>* vs, Matrix<int>* trips,
                                 double alpha_w, double alpha_s) {
    // std::cout << "generateWasteRockSolution(...)" << std::endl;

    // =====================
    // Mark the used shovels
    // =====================

    vector<bool> usedShovel(opm.numCarregs, false);

    for (unsigned i = 0; i < vs->size(); i++)
      if (vs->at(i) >= 0) usedShovel[vs->at(i)] = true;

    // ========================================
    // Get max productivity of non-used shovels
    // ========================================

    std::vector<std::pair<int, double> > shovelCap;

    for (int s = 0; s < opm.numCarregs; s++)
      if (!usedShovel[s]) shovelCap.push_back(std::make_pair(s, opm.cMax[s]));

    // sort the shovels by productivity
    sort(shovelCap.rbegin(), shovelCap.rend(), sort_pred);

    // std::cout << "shovelCap: "<<shovelCap<<endl;

    // =================
    // Trucks timelimit
    // =================

    vector<double> truckTime(opm.numCaminhoes,
                             0);  // total used time of each truck
    const double maxTime = 60 * opm.txUtilCam;

    // ==================================
    // Order trucks by maximum capacity
    // ==================================

    std::vector<std::pair<int, double> > truckCap;  // capacity of each truck

    for (int c = 0; c < opm.numCaminhoes; c++)
      truckCap.push_back(std::make_pair(c, opm.l[c]));

    sort(truckCap.rbegin(), truckCap.rend(), sort_pred);

    // std::cout << "truckCap: "<<truckCap<<endl;

    // ========================================
    // Get qu of waste rock pits
    // ========================================

    std::vector<std::pair<int, double> > wasteRockPitQu;

    for (int i = 0; i < opm.numFrentes; i++)
      if (!opm.isMin[i]) wasteRockPitQu.push_back(std::make_pair(i, opm.qu[i]));

    // sort the waste rock pits by qu
    sort(wasteRockPitQu.rbegin(), wasteRockPitQu.rend(), sort_pred);

    // std::cout << "wasteRockPitQu: "<<wasteRockPitQu<<endl;

    // ====================================
    //   Meet waste rock production goal
    // ====================================

    vector<double> x(opm.numFrentes, 0);  // production of each pit

    // update previously used resources
    for (int i = 0; i < opm.numFrentes; i++) {
      int dx = 0;
      for (int c = 0; c < opm.numCaminhoes; c++)
        if ((*trips)(i, c) > 0) {
          if ((*vs)[i] < 0) std::cout << "ERROR! No Shovel and Many Trips!" << std::endl;
          dx += (*trips)(i, c) * opm.l[c];
          truckTime[c] += opm.tempo[i];
          std::cout << "Update!" << std::endl;
        }
      x[i] += dx;
    }

    double wasteRockProd = 0;  // waste rock production

    // std::cout << "waste rock production goal = "<<opm.pr_est<<endl;

    while (wasteRockProd < opm.pr_est) {
      // get one of the better waste rock pits

      int numGoodPits = ceil(alpha_w * wasteRockPitQu.size());

      // std::cout << "there are "<<numGoodPits<<" good waste rock pits"<<endl;

      if (numGoodPits > 0) {
        int w_r = rg.rand(numGoodPits);
        int w_id = wasteRockPitQu[w_r].first;

        if (vs->at(w_id) < 0)  // no shovel
        {
          // get one of the better shovels

          int numGoodShovels = ceil(alpha_s * shovelCap.size());
          // std::cout << "there are "<<numGoodShovels<<" good shovels"<<endl;

          if (numGoodShovels > 0) {
            int s_r = rg.rand(numGoodShovels);
            int s_id = shovelCap[s_r].first;
            // allocate s_id at w_id
            (*vs)[w_id] = s_id;
            // remove s_id
            shovelCap.erase(shovelCap.begin() + s_r);
          } else  // no shovels! cant use this waste rock pit
          {
            wasteRockPitQu.erase(wasteRockPitQu.begin() + w_r);
            continue;
          }
        }

        // good pit and good shovel
        int s_id = vs->at(w_id);

        bool erasePit = true;

        // for each truck, try to insert another trip
        for (int c = 0; c < opm.numCaminhoes; c++)
          if (opm.isCompatible(truckCap[c].first, s_id)) {
            int t_id = truckCap[c].first;     // truck id
            double cap = truckCap[c].second;  // truck capacity
            double qu = opm.qu[w_id];         // qu of the waste rock pit
            double prodMax = opm.cMax[s_id];  // max productivity of the shovel
            while (wasteRockProd < opm.pr_est &&  // production goal
                   (x[w_id] + cap) <=
                       min(prodMax, qu) &&  // max productivity or qu
                   (truckTime[t_id] + opm.tempo[w_id]) <=
                       maxTime)  // max usage time
            {
              erasePit = false;        // wont erase the pit
              (*trips)(w_id, t_id)++;  // add one trip
              x[w_id] += cap;          // increase productivity
              wasteRockProd += cap;    // increase waste rock production
              truckTime[t_id] += opm.tempo[w_id];  // increase usage time
            }
          }

        if (erasePit) wasteRockPitQu.erase(wasteRockPitQu.begin() + w_r);
      } else {
        // std::cout << "Cant generate a feasible solution!"<<endl;
        break;
      }
    }

    // std::cout << "waste rock production = " << wasteRockProd << std::endl;
    // std::cout << "end generateWasteRockSolution" << std::endl;
  }

  void generateOreSolution(std::vector<int>* vs, Matrix<int>* trips, double alpha_o,
                           double alpha_s) {
    // std::cout << "generateOreSolution(...)" << std::endl;

    // =====================
    // Mark the used shovels
    // =====================

    vector<bool> usedShovel(opm.numCarregs, false);

    for (unsigned i = 0; i < vs->size(); i++)
      if (vs->at(i) >= 0) usedShovel[vs->at(i)] = true;

    // ========================================
    // Get max productivity of non-used shovels
    // ========================================

    std::vector<std::pair<int, double> > shovelCap;

    for (int s = 0; s < opm.numCarregs; s++)
      if (!usedShovel[s]) shovelCap.push_back(std::make_pair(s, opm.cMax[s]));

    // sort the shovels by productivity
    sort(shovelCap.rbegin(), shovelCap.rend(), sort_pred);

    // std::cout << "shovelCap: "<<shovelCap<<endl;

    // =================
    // Trucks timelimit
    // =================

    vector<double> truckTime(opm.numCaminhoes,
                             0);  // total used time of each truck
    const double maxTime = 60 * opm.txUtilCam;

    // ==================================
    // Order trucks by maximum capacity
    // ==================================

    double truckMinCap = 10000000;  // INF

    std::vector<std::pair<int, double> > truckCap;  // capacity of each truck

    for (int c = 0; c < opm.numCaminhoes; c++) {
      if (opm.l[c] < truckMinCap) truckMinCap = opm.l[c];
      truckCap.push_back(std::make_pair(c, opm.l[c]));
    }

    sort(truckCap.begin(), truckCap.end(), sort_pred);

    // std::cout << "truckCap: "<<truckCap<<endl;
    // std::cout << "truckMinCap = "<<truckMinCap<<endl;

    // ====================================
    //   Control parameters
    // ====================================

    vector<double> controlParamTons(opm.numParams, 0);  // in tons
    vector<double> controlParamPerc(opm.numParams, 0);  // in percentage

    // ====================================
    //   Meet ore production goal
    // ====================================

    vector<double> x(opm.numFrentes, 0);  // production of each pit

    // ====================================
    //   Update previously used resources
    // ====================================

    for (int i = 0; i < opm.numFrentes; i++) {
      int dx = 0;
      for (int c = 0; c < opm.numCaminhoes; c++)
        if ((*trips)(i, c) > 0) {
          if ((*vs)[i] < 0) std::cout << "ERROR! No Shovel and Many Trips!" << std::endl;
          dx += (*trips)(i, c) * opm.l[c];
          truckTime[c] += opm.tempo[i] * (*trips)(i, c);

          // if(opm.isMin[i]) // Update control parameters TODO
          //{
          // }
        }
      x[i] += dx;
    }

    // ========================================
    //   Create ore pits
    // ========================================

    std::vector<std::pair<int, double> > orePits;

    for (int i = 0; i < opm.numFrentes; i++)
      if (opm.isMin[i]) orePits.push_back(std::make_pair(i, 0));

    // =========================================

    double oreProd = 0;  // waste rock production

    // std::cout << "ore production goal = "<<opm.pr_min<<endl;

    // std::cout << "orePits: "<<orePits<<endl;

    while (oreProd < opm.pr_min) {
      for (unsigned o = 0; o < orePits.size(); o++) {
        int i = orePits[o].first;

        // std::cout << "orePit "<<i<<endl;
        // std::cout << "teores: ";
        for (int p = 0; p < opm.numParams; p++)
          if (oreProd > 0) {
            // std::cout <<(*opm.teor)(i,p) << " ";
            controlParamPerc[p] =
                (controlParamTons[p] + (*opm.teor)(i, p) * truckMinCap) /
                oreProd;
          } else
            controlParamPerc[p] = 0;
        // std::cout << std::endl;

        // std::cout << "controlParamPerc: "<<controlParamPerc<<endl;

        double dif = 0;

        for (int p = 0; p < opm.numParams; p++) {
          // std::cout << absval(controlParamPerc[p]-opm.tr[p]) << " ";
          dif += absval(controlParamPerc[p] - opm.tr[p]);
        }
        // std::cout << std::endl;

        orePits[o].second = dif;
      }

      sort(orePits.begin(), orePits.end(), sort_pred);

      // get one of the better ore pits

      int numGoodPits = ceil(alpha_o * orePits.size());

      // std::cout << "there are "<<numGoodPits<<" good ore pits"<<endl;

      if (numGoodPits > 0) {
        double sumBias = 0;
        for (int b = 1; b <= numGoodPits; b++) sumBias += 1.0 / b;

        int sumBiasInt = sumBias * 100000;

        double o_r = rg.rand(sumBiasInt) / 100000.0;

        int cand = numGoodPits - 1;  // the last candidate

        for (int b = 1; b <= numGoodPits; b++) {
          o_r -= 1.0 / b;

          if (o_r < 0) {
            cand = b - 1;
            break;
          }
        }

        // std::cout << "orePits: "<<orePits<<endl;
        // std::cout << "Chosen Cand: "<<cand<<" value: "<<
        // orePits[cand].second<<endl;

        int o_id = orePits[cand].first;

        if (vs->at(o_id) < 0)  // no shovel
        {
          // get one of the better shovels

          int numGoodShovels = ceil(alpha_s * shovelCap.size());
          // std::cout << "there are "<<numGoodShovels<<" good shovels"<<endl;

          if (numGoodShovels > 0) {
            int s_r = rg.rand(numGoodShovels);
            int s_id = shovelCap[s_r].first;
            // allocate s_id at w_id
            (*vs)[o_id] = s_id;
            // remove s_id
            shovelCap.erase(shovelCap.begin() + s_r);
          } else  // no shovels! cant use this ore pit
          {
            orePits.erase(orePits.begin() + cand);
            // std::cout << "Delete ore pit "<<o_id<<"
            // |orePits|="<<orePits.size()<<endl;
            continue;
          }
        }

        // good pit and good shovel
        int s_id = vs->at(o_id);

        bool erasePit = true;

        // for each truck, try to insert another trip
        for (int c = 0; c < opm.numCaminhoes; c++)
          if (opm.isCompatible(truckCap[c].first, s_id)) {
            int t_id = truckCap[c].first;     // truck id
            double cap = truckCap[c].second;  // truck capacity
            double qu = opm.qu[o_id];         // qu of the waste rock pit
            double prodMax = opm.cMax[s_id];  // max productivity of the shovel
            if (oreProd < opm.pr_min &&       // production goal
                (x[o_id] + cap) <=
                    min(prodMax, qu) &&  // max productivity or qu
                (truckTime[t_id] + opm.tempo[o_id]) <=
                    maxTime)  // max usage time
            {
              erasePit = false;        // wont erase the pit
              (*trips)(o_id, t_id)++;  // add one trip
              x[o_id] += cap;          // increase productivity

              oreProd += cap;  // increase waste rock production

              for (int p = 0; p < opm.numParams; p++)
                controlParamTons[p] += (*opm.teor)(o_id, p) * cap;

              truckTime[t_id] += opm.tempo[o_id];  // increase usage time
              break;
            }
          }

        if (erasePit) {
          orePits.erase(orePits.begin() + cand);
          // std::cout << "Delete ore pit "<<o_id<<"
          // |orePits|="<<orePits.size()<<endl;
        }
      } else {
        // std::cout << "Cant generate a feasible solution!"<<endl;
        break;
      }
    }

    // std::cout << "ore production = " << oreProd << std::endl;
    // std::cout << "end generateOreSolution" << std::endl;
  }
};
}  // namespace POLAD
#endif /*OPM_INITIALSOLUTION_GreedyRandomized_HPP_*/
