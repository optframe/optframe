#ifndef SVRPDSP_CONSTRUCTIVE_TSPOptimal_HPP_
#define SVRPDSP_CONSTRUCTIVE_TSPOptimal_HPP_

#include <OptFrame/Core/Constructive.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp"

#include <stdlib.h>

#include <algorithm>
#include <list>

#include "ADS.h"
#include "Evaluator.hpp"
#include "MySolution.hpp"
#include "ProblemInstance.hpp"
#include "Representation.h"

using namespace std;

namespace SVRPDSP {

class ConstructiveTSPOptimal
    : public Constructive<RepSVRPDSP, AdsSVRPDSP, MySolution> {
 private:
  ProblemInstance& pSVRPDSP;
  Scanner& tsp;
  Scanner& knapsack;

 public:
  ConstructiveTSPOptimal(ProblemInstance& _pSVRPDSP, Scanner& _tsp,
                         Scanner& _knapsack)
      : pSVRPDSP(_pSVRPDSP), tsp(_tsp), knapsack(_knapsack) {}

  virtual ~ConstructiveTSPOptimal() {}

  MySolution* generateSolution(double timelimit) override {
    RepSVRPDSP rep;

    rep.push_back(0);  // depot

    int n_k = knapsack.nextInt();
    vector<bool> knp(n_k, false);

    for (int i = 0; i < n_k; i++)
      if (knapsack.nextInt()) knp[i] = true;

    int n_d = tsp.nextInt() - 1;
    int dep = tsp.nextInt();  // drop depot

    if (dep != 0) {
      std::cout << "ConstructiveTSPOptimal: NOT A DEPOT!!!" << std::endl;
      exit(1);
    }

    int count = 0;  // count deliveries to check if correct!

    for (int i = 0; i < n_d; i++) {
      int d = tsp.nextInt();
      count++;

      rep.push_back(d);

      if (knp[d]) rep.push_back(d + pSVRPDSP.n);
    }

    rep.push_back(0);  // depot

    for (int i = 1; i < knp.size(); i++)
      if (!knp[i]) rep.push_back(i + pSVRPDSP.n);

    /*
    std::cout << "ANALYSIS:" << std::endl;
    std::cout << "NUM_TSP: " << n_d << std::endl;
    std::cout << "NUM_KNP: " << (n_k-1) << std::endl;
    std::cout << "SIZE: " << rep.size() << std::endl;

    vector<bool> d(n_d, false);
    for(unsigned i=0; i<rep.size(); i++)
            if(rep[i]>0 && rep[i]<=pSVRPDSP.n)
                    d[rep[i]-1] = true;

    std::cout << "pSVRPDSP.n: " << pSVRPDSP.n << std::endl;
    std::cout << "d: " << d << std::endl;
    std::cout << "count: " << count << std::endl;
    */

    MySolution* s = new MySolution(rep);

    if (!s->syncADS(pSVRPDSP)) {
      std::cout << "error syncronizing ADS (Const. TSPOpt)" << std::endl;
      std::cout << "rep: " << rep << std::endl;
      exit(1);
    }

    return s;
  }
};

}  // namespace SVRPDSP

#endif /*SVRPDSP_CONTRUCTIVE_TSPOptimal_HPP_*/
