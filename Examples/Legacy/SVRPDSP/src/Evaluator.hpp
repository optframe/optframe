#ifndef SVRPDSP_EVALUATOR_HPP_
#define SVRPDSP_EVALUATOR_HPP_

// #include <cmath>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>

#include "ADS.h"
#include "Delta.h"
#include "MySolution.hpp"
#include "ProblemInstance.hpp"
#include "Representation.h"

#define EPSILON_SVRPDSP 0.0001

namespace SVRPDSP {

class SVRPDSPEvaluator : public Evaluator<RepSVRPDSP, AdsSVRPDSP, MySolution> {
 private:
  ProblemInstance& pSVRPDSP;

  static const bool verbose = false;

 public:
  using Evaluator<RepSVRPDSP, AdsSVRPDSP, MySolution>::evaluate;

  SVRPDSPEvaluator(ProblemInstance& _pSVRPDSP)
      : pSVRPDSP(_pSVRPDSP),
        Evaluator<RepSVRPDSP, AdsSVRPDSP, MySolution>(true)  // ALLOW COSTS
  {}

  bool basicCheck(const RepSVRPDSP& rep, const AdsSVRPDSP& ads) {
    if (rep.size() == 0) {
      std::cout << "Evaluator Error: rep.size() == 0!" << std::endl;
      return false;
    }

    if (ads.zero >= rep.size()) {
      std::cout << "Evaluator Error: zero is bigger than rep.size()! Incorrect ADS!"
           << std::endl;
      std::cout << "rep: " << rep << std::endl;
      return false;
    }

    if (rep[ads.zero] != 0) {
      std::cout << "Evaluator Error: problem with zero in rep[" << ads.zero
           << "] = " << rep[ads.zero] << "! Incorrect ADS!" << std::endl;
      std::cout << "rep: " << rep << std::endl;
      return false;
    }

    vector<int> idx(pSVRPDSP.dN, -1);
    for (unsigned i = 0; i < rep.size(); i++)
      if (rep[i] != 0) {
        if (idx.at(rep[i]) >= 0) {
          std::cout << "repetition of element '" << rep[i] << "' at i=" << i
               << " and " << idx[rep[i]] << std::endl;
          std::cout << "REP: " << rep << std::endl;
          exit(1);
        }
        idx.at(rep[i]) = i;
      }

    int count_d = 0;
    for (unsigned i = 1; i <= ads.zero; i++)
      if (rep[i] != 0) {
        // delivery
        if (rep[i] <= pSVRPDSP.n) count_d++;
      }

    if (count_d != pSVRPDSP.n)  // using penalization
    {
      std::cout << "Evaluator Wrong number of delivery customers! It is " << count_d
           << " but should be " << pSVRPDSP.n << "!" << std::endl;
      exit(1);
    }

    return true;
  }

  Evaluation<> evaluate(const RepSVRPDSP& rep) {
    std::cout << "DEPRECATED:DO NOT USE THIS METHOD!" << std::endl;
    exit(1);
    const AdsSVRPDSP ads1;
    return evaluate(rep, &ads1);
  }

  Evaluation<> evaluate(const RepSVRPDSP& rep,
                        const AdsSVRPDSP* _ads1) override {
    const AdsSVRPDSP& ads1 = *_ads1;
    AdsSVRPDSP ads = ads1;
    MySolution::syncADS(pSVRPDSP, rep, ads);
    if (verbose) {
      std::cout << "evaluate(rep,ads)" << std::endl;
      std::cout << "ZERO IS: " << ads.zero << " rep: " << rep << std::endl;
      ads.print();
    }

#ifdef ALL_INTEGER
    long long int INF_CAP_WEIGHT = 100000;
    long long int INF_NO_DELIVERY_WEIGHT = 100000 * 100;

    long long int fo = 0;
    long long int fo_c = 0;
    long long int fo_r = 0;

    long long int fo_inf = 0;
    long long int sum_inf = 0;

    int q = pSVRPDSP.Q;
#endif
#ifndef ALL_INTEGER
    double INF_CAP_WEIGHT = 100000;
    double INF_NO_DELIVERY_WEIGHT = 100000 * 1000;

    double fo = 0;
    double fo_c = 0;
    double fo_r = 0;

    double fo_inf = 0;
    double sum_inf = 0;

    double q = pSVRPDSP.Q;
#endif

    // basic checks regarding position of zero
    assert(basicCheck(rep, ads));

    double max = q;
    int i_max = 0;

    for (unsigned i = 1; i <= ads.zero; i++) {
      // std::cout << "i=" << i << std::endl;
      q += pSVRPDSP.dp[rep[i]];
      q -= pSVRPDSP.dd[rep[i]];

      if ((q != ads.load[i]) && (rep[i] != 0))  // do not check depot
      {
        std::cout << "Evaluator DIFFERENT Q VALUE IN i=" << i << " q=" << q
             << " ads.load[i]=" << ads.load[i] << std::endl;
        exit(1);
      }

      if (q < 0) {
        std::cout << "Evaluator ERROR: negative capacity!" << std::endl;
        exit(1);
      }

      if ((q > max) && (rep[i] != 0))  // epsilon
      {
        max = q;
        i_max = i;
      }

      fo_c += pSVRPDSP.dc(rep[i - 1], rep[i]);
      if (verbose)
        std::cout << "+d(" << rep[i - 1] << "," << rep[i]
             << ")=" << pSVRPDSP.dc(rep[i - 1], rep[i]) << " ";

      if (rep[i] != 0) {
        // delivery
        if (rep[i] <= pSVRPDSP.n) {
          // nothing to do
        } else  // pickup
        {
          fo_r += pSVRPDSP.dr[rep[i]];
          if (verbose)
            std::cout << "PICKUP(" << rep[i] << ")=" << pSVRPDSP.dr[rep[i]] << " ";
        }
      }
    }

    if (max > pSVRPDSP.Q) {
      fo_inf += (max - pSVRPDSP.Q) * INF_CAP_WEIGHT;  // INFEASIBLE!
      sum_inf += (max - pSVRPDSP.Q);
      if (verbose)
        std::cout << "Evaluator inf: " << (max - pSVRPDSP.Q) << " in " << rep[i_max]
             << " => " << fo_inf << std::endl;
    }

    fo = fo_c - fo_r;

    if (verbose) {
      std::cout << "TOTAL = " << fo_c << std::endl;
      std::cout << "Evaluator TOTAL INF1+INF2: " << fo_inf << std::endl;
      std::cout << "fo_c=" << fo_c << " fo_r=" << fo_r << std::endl;
    }

    return *new Evaluation(fo, fo_inf);
  }

  virtual bool betterThan(double a, double b) {
    // MINIMIZATION
    return (a < (b - EPSILON_SVRPDSP));
  }

  virtual bool isMinimization() const { return true; }
};

}  // namespace SVRPDSP

#endif /*SVRPDSP_EVALUATOR_HPP_*/
