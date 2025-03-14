#ifndef MODM_EVALUATOR_H_
#define MODM_EVALUATOR_H_

// #include <cmath>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Timer.hpp>

#include "ADS.h"
#include "ADSManager.h"
#include "DeltaStructure.h"
#include "Evaluation.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

#define EPSILON_MODM 0.0001

namespace MODM {

class MODMEvaluator
    : public Evaluator<SolutionMODM, EvaluationMODM, ESolutionMODM> {
 private:
  ProblemInstance& pMODM;
  MODMADSManager& adsMan;
  //	double totalTime;
  //	double totalTimeEC;
  //	int nEval;
  // Your private vars

 public:
  MODMEvaluator(ProblemInstance& _pMODM, MODMADSManager& _adsMan)
      : Evaluator(new Maximization()), pMODM{_pMODM}, adsMan{_adsMan} {
    //		totalTime = 0;
    //		totalTimeEC = 0;
    //		nEval = 0;
    // Put the rest of your code here
  }

  //	double getAverageTime()
  //	{
  //		return totalTime / nEval;
  //	}
  //
  //	double getAverageTimeEvalComplete()
  //	{
  //		return totalTimeEC / nEval;
  //	}

  virtual ~MODMEvaluator() {}

  Evaluation<> evaluate(const SolutionMODM& s) override {
    const RepMODM& rep = s.getR();
    const AdsMODM& ads = s.getADS();

    // AdsMODM ads;
    // adsMan.initializeADS(rep, ads);
    return evaluateRADS(rep, &ads);
  }

  void evaluatorCheck(const RepMODM& rep, const AdsMODM& ads, double fo,
                      double foInv) {
    int maxC = pMODM.getNumberOfClients();
    int maxP = pMODM.getNumberOfProducts();
    AdsMODM adsCheck;
    adsMan.initializeADS(rep, adsCheck);
    bool comp = adsMan.compareADS(adsCheck, ads);
    double foRevenueAds = 0;
    double foBudgetAds = 0;
    double foFixedCostAds = 0;

    if (comp == false) {
      std::cout << "bug on ADS compare!" << std::endl;
      getchar();
    }

    Timer tEC;

    int foRevenue = 0;
    int foCost = 0;
    int foFixedCost = 0;
    int foInvMin;
    for (int p = 0; p < maxP; p++) {
      int nOffers = 0;
      for (int c = 0; c < maxC; c++) {
        int offer = rep[c][p];
        if (offer == 1) {
          foCost += pMODM.getCost(c, p);
          foRevenue += pMODM.getRevenue(c, p);
          nOffers++;
        }
      }
      if (nOffers > 0) foFixedCost += pMODM.getProductFixedCost(p);

      if (nOffers < pMODM.getProductMinClients(p))
        foInvMin = pMODM.getProductMinClients(p) - nOffers;
    }

    //		totalTimeEC += tEC.now();

    for (int p = 0; p < maxP; p++) {
      foRevenueAds += adsCheck.totalRevenue[p];
      foBudgetAds += adsCheck.totalCost[p];

      if (ads.productOffers[p] > 0) {
        foFixedCostAds += pMODM.getProductFixedCost(p);
      }
    }

    double foCheck = foRevenue - foCost - foFixedCost;
    double foCheckADS = foRevenueAds - foBudgetAds - foFixedCostAds;

    if (foCheck != fo || foCheck != foCheckADS) {
      std::cout << "Bug on evaluator ! Diff evals" << std::endl;
      std::cout << "foCheck = " << foCheck << std::endl;
      std::cout << "foCheckADS = " << foCheckADS << std::endl;
      std::cout << "fo = " << fo << std::endl;
      /*			cout << "foRevenue = " << foRevenue << std::endl;
                      std::cout << "foRevenueAds = " << foRevenueAds << std::endl;
                      std::cout << "foCost = " << foCost << std::endl;
                      std::cout << "foCostAds = " << foBudgetAds << std::endl;
                      std::cout << "foFixedCost = " << foFixedCost << std::endl;
                      std::cout << "foFixedCostAds = " << foFixedCostAds << std::endl;*/
      getchar();
    }
  }

  Evaluation<> evaluateRADS(const RepMODM& rep, const AdsMODM* _ads) {
    const AdsMODM& ads = *_ads;
    Timer t;
    // 'rep' is the representation of the solution
    int maxC = pMODM.getNumberOfClients();
    int n = pMODM.getNumberOfProducts();

    double fo = 0;  // Evaluation<> Function Value
    double foRevenue = 0;
    double foBudget = 0;
    double foFixedCost = 0;
    double foInvMin = 0;
    double foInvBud = 0;
    double foInvHR = 0;
    double foInvMaxOffersC = 0;

    for (int product = 0; product < n; product++) {
      foRevenue += ads.totalRevenue[product];
      foBudget += ads.totalCost[product];

      if (ads.productOffers[product] > 0) {
        foFixedCost += pMODM.getProductFixedCost(product);

        if (pMODM.getProductMinClients(product) > ads.productOffers[product]) {
          foInvMin +=
              pMODM.getProductMinClients(product) - ads.productOffers[product];
          // std::cout << "BUG ON EVALUATOR - NUMERO MINIMO CLIENTE NAO FOI
          // RESPEITADO!!!" << std::endl; std::cout << "ads.productOffers[product] = " <<
          // ads.productOffers[product] << std::endl; std::cout <<
          // "pMODM.minClients[product] = " << pMODM.minClients[product] <<
          // std::endl; getchar();
        }
      }

      if (ads.totalCost[product] > pMODM.getProductBudget(product)) {
        foInvBud += ads.totalCost[product] - pMODM.getProductBudget(product);
        // std::cout << "BUG ON EVALUATOR - BUDGET NAO FOI RESPEITADO!!!" << std::endl;
        // std::cout << "ads.totalCost[product] = " << ads.totalCost[product] <<
        // std::endl; std::cout << "pMODM.getProductBudget(product) = " <<
        // pMODM.getProductBudget(product) << std::endl; getchar();
      }
    }

    for (int c = 0; c < maxC; c++) {
      if (ads.clientOffers[c] > pMODM.getClientMaxOffers(c)) {
        // std::cout << "BUG ON EVALUATOR - Cliente MAX NAO FOI RESPEITADO!!!" <<
        // std::endl; std::cout << "ads.clientOffers[c] = " << ads.clientOffers[c] <<
        // std::endl; std::cout << "pMODM.getClientMaxOffers(c) = " <<
        // pMODM.getClientMaxOffers(c) << std::endl;
        foInvMaxOffersC += ads.clientOffers[c] - pMODM.getClientMaxOffers(c);
        // getchar();
      }
    }

    // Verificar inviabilidades
    if (foRevenue < ((1 + pMODM.getHurdleRate()) * (foBudget + foFixedCost))) {
      foInvHR = 1;
      // std::cout << "BUG ON EVALUATOR - LUCRO MENOR QUE O ESPERADO!!!" << std::endl;
      // getchar();
    }

    fo = foRevenue - foBudget - foFixedCost;
    // double foInv = foInvHR * (-100000) + foInvBud * (-100000) + foInvMin *
    // (-20000) + foInvMaxOffersC * (-10000);
    double foInv = foInvHR * (-100000) + foInvBud * (-100000) +
                   foInvMin * (-20000) + foInvMaxOffersC * (-1000);

    //		if(foInv != 0)
    //		{
    //			cout<<foInvHR<<endl;
    //			cout<<foInvBud<<endl;
    //			cout<<foInvMin<<endl;
    //			cout<<foInvMaxOffersC<<endl;
    //			getchar();
    //		}
    // std::cout << "FO = " << fo << std::endl;

    /*
               FILE* arquivo = fopen("log.txt", "a");
               if (!arquivo)
               {
               std::cout << "ERRO: falha ao criar arquivo \"log.txt\"" << std::endl;
               }
               else
               {

               fprintf(arquivo, "%.4f\t \n", fo);

               fclose(arquivo);
               }*/

    //		totalTime += t.now();
    //
    //		nEval++;

    // evaluatorCheck(rep, ads, fo, foInv);

    return Evaluation(fo, foInv);
  }

  void exportEvaluation(const RepMODM& rep, const AdsMODM& ads, string filename,
                        string outFile) {
    Evaluation<> e = evaluateRADS(rep, &ads);

    double fo = e.evaluation();

    FILE* f = fopen(outFile.c_str(), "w");

    fprintf(f, "%s \t %.2f \n", filename.c_str(), fo);

    fclose(f);
  }

  /*
       virtual bool betterThan(double a, double b) override
       {
               // MAXIMIZATION
               return (a > (b + EPSILON_MODM));
       }
  */

  virtual bool isMinimization() const override { return false; }
};

}  // namespace MODM

#endif /*MODM_EVALUATOR_H_*/
