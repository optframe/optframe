#ifndef MODM_EVALUATORROBUSTNESS_H_
#define MODM_EVALUATORROBUSTNESS_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"
#include "../../OptFrame/Timer.hpp"
#include "Representation.h"
#include "ADS.h"
#include "../../OptFrame/RandGen.hpp"

#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#include "ADSManager.h"

#define EPSILON_MODM 0.0001

namespace MODM
{

class MODMRobustnessEvaluator: public Evaluator<RepMODM, AdsMODM>
{
private:
	ProblemInstance& pMODM;
	MODMADSManager& adsMan;
	double totalTime;
	double totalTimeEC;
	int nEval;
	RandGen& rg;
	// Your private vars

public:
	MODMRobustnessEvaluator(ProblemInstance& _pMODM, MODMADSManager& _adsMan, RandGen& _rg) : // If necessary, add more parameters
			pMODM(_pMODM), adsMan(_adsMan), rg(_rg)
	{
		totalTime = 0;
		totalTimeEC = 0;
		nEval = 0;
		// Put the rest of your code here
	}

	double getAverageTime()
	{
		return totalTime / nEval;
	}

	double getAverageTimeEvalComplete()
	{
		return totalTimeEC / nEval;
	}

	virtual ~MODMRobustnessEvaluator()
	{
	}

	EvaluationMODM& evaluate(const RepMODM& rep)
	{
		AdsMODM ads;
		adsMan.initializeADS(rep, ads);
		return evaluate(rep, ads);
	}

	vector<int> checkNClientsPerCategory(const RepMODM& rep, const AdsMODM& ads)
	{
		vector<int> nPerCat(6, 0); // vector that contains the number of number in each desired category of volatity

		int maxC = pMODM.getNumberOfClients();
		int maxP = pMODM.getNumberOfProducts();


		int nTotalClients = 0;
		for (int p = 0; p < maxP; p++)
		{
			nTotalClients += ads.productOffers[p];
			for (int c = 0; c < maxC; c++)
				if (rep[c][p] == true)
				{

					double rev = pMODM.getRevenue(c, p);

					double clientRisk = 0;
					if (rev >= 0 && rev < 3)
						nPerCat[0]++;

					if (rev >= 3 && rev < 5)
						nPerCat[1]++;

					if (rev >= 5 && rev < 8)
						nPerCat[2]++;

					if (rev >= 8 && rev < 11)
						nPerCat[3]++;

					if (rev >= 11 && rev < 14)
						nPerCat[4]++;

					if (rev >= 14)
						nPerCat[5]++;

				}
		}

//		cout<<nPerCat<<endl;
//		getchar();

		nPerCat.push_back(nTotalClients);

		return nPerCat;

	}

	EvaluationMODM& evaluate(const RepMODM& rep, const AdsMODM& ads)
	{
		Timer t;
		// 'rep' is the representation of the solution
		int maxC = pMODM.getNumberOfClients();
		int maxP = pMODM.getNumberOfProducts();

		double fo = 0; // Evaluation Function Value
		double foRevenue = 0;
		double foBudget = 0;
		double foFixedCost = 0;
		double foInvMin = 0;
		double foInvBud = 0;
		double foInvHR = 0;
		double foInvMaxOffersC = 0;

		for (int product = 0; product < maxP; product++)
		{
			foRevenue += ads.totalRevenue[product];
			foBudget += ads.totalCost[product];

			if (ads.productOffers[product] > 0)
			{
				foFixedCost += pMODM.getProductFixedCost(product);

				if (pMODM.getProductMinClients(product) > ads.productOffers[product])
				{
					foInvMin += pMODM.getProductMinClients(product) - ads.productOffers[product];
					//cout << "BUG ON EVALUATOR - NUMERO MINIMO CLIENTE NAO FOI RESPEITADO!!!" << endl;
					//cout << "ads.productOffers[product] = " << ads.productOffers[product] << endl;
					//cout << "pMODM.minClients[product] = " << pMODM.minClients[product] << endl;
					//getchar();
				}
			}

			if (ads.totalCost[product] > pMODM.getProductBudget(product))
			{
				foInvBud += ads.totalCost[product] - pMODM.getProductBudget(product);
				//cout << "BUG ON EVALUATOR - BUDGET NAO FOI RESPEITADO!!!" << endl;
				//cout << "ads.totalCost[product] = " << ads.totalCost[product] << endl;
				//cout << "pMODM.getProductBudget(product) = " << pMODM.getProductBudget(product) << endl;
				//getchar();
			}

		}

		for (int c = 0; c < maxC; c++)
		{
			if (ads.clientOffers[c] > pMODM.getClientMaxOffers(c))
			{
				//cout << "BUG ON EVALUATOR - Cliente MAX NAO FOI RESPEITADO!!!" << endl;
				//cout << "ads.clientOffers[c] = " << ads.clientOffers[c] << endl;
				//cout << "pMODM.getClientMaxOffers(c) = " << pMODM.getClientMaxOffers(c) << endl;
				foInvMaxOffersC += ads.clientOffers[c] - pMODM.getClientMaxOffers(c);
				//getchar();
			}
		}

		//Verificar inviabilidades
		if (foRevenue < ((1 + pMODM.getHurdleRate()) * (foBudget + foFixedCost)))
		{
			foInvHR = 1;
			//cout << "BUG ON EVALUATOR - LUCRO MENOR QUE O ESPERADO!!!" << endl;
			//getchar();
		}

		fo = foRevenue - foBudget - foFixedCost;

		double foInv = 0;

		//calculating variable profit
//		int nMonteCarlo = 10;
//		double variableProfit = 0;

//		for (int i = 0; i < nMonteCarlo; i++)
//			for (int p = 0; p < maxP; p++)
//				for (int c = 0; c < maxC; c++)
//				{
//					if (rep[c][p] == true)
//					{
//						int clientUncentainty;
//						if (c < 300)
//							clientUncentainty = rg.randG(10, 7); // podem aumentar o lucro mas alta variabilidade - risco alto
//
//						if (c >= 300 && c < 500)
//							clientUncentainty = rg.randG(6, 4); // podem aumentar o lucro mas media variabilidade - risco medio
//
//						if (c >= 500 && c < 800)
//							clientUncentainty = rg.randG(0, 1); // clientes medios com baixa variabilidade - normal
//
//						if (c >= 800)
//							clientUncentainty = rg.randG(-2, 0.5); // clientes normais - normal
//
//						variableProfit += clientUncentainty;
//
//					}
//
//				}

		double totalVolatibility = 0;
		for (int p = 0; p < maxP; p++)
			for (int c = 0; c < maxC; c++)
				if (rep[c][p] == true)
				{
					double rev = pMODM.getRevenue(c, p);

					double clientRisk = 0;
					if (rev >= 0 && rev < 3)
						clientRisk = 0.01;

					if (rev >= 3 && rev < 5)
						clientRisk = 0.05;

					if (rev >= 5 && rev < 8)
						clientRisk = 0.1;

					if (rev >= 8 && rev < 11)
						clientRisk = 0.25;

					if (rev >= 11 && rev < 14)
						clientRisk = 0.4;

					if (rev >= 14)
						clientRisk = 0.6;

					totalVolatibility += clientRisk;
				}

		int nAcceptedClients = 0;
		for (int p = 0; p < maxP; p++)
			nAcceptedClients += ads.productOffers[p];

		double meanVolabitility = totalVolatibility / nAcceptedClients;

		double riskFree = pMODM.getHurdleRate();
		double campaingCosts = foBudget + foFixedCost;
		double campaingProfit = (foRevenue - campaingCosts) / campaingCosts;

		double sharpeRatio = (campaingProfit - riskFree) / meanVolabitility;

//		cout << "nAcceptedClients = " << nAcceptedClients << endl;
//		cout << "foRevenue = " << foRevenue << endl;
//		cout << "foBudget + foFixedCost = " << foBudget + foFixedCost << endl;
//		cout << "campaingProfit = " << campaingProfit << endl;
//		cout << "sharpeRatio = " << sharpeRatio << endl;
//		getchar();
//
//		variableProfit /= nMonteCarlo;

		return *new EvaluationMODM(sharpeRatio, foInv);
	}

	void exportEvaluation(const RepMODM& rep, const AdsMODM& ads, string filename, string outFile)
	{
		EvaluationMODM& e = evaluate(rep, ads);

		double fo = e.evaluation();

		FILE* f = fopen(outFile.c_str(), "w");

		fprintf(f, "%s \t %.2f \n", filename.c_str(), fo);

		fclose(f);
	}

	virtual bool betterThan(double a, double b)
	{
		// MAXIMIZATION
		return (a > (b + EPSILON_MODM));
	}

	virtual bool isMinimization() const
	{
		return false;
	}
};

}

#endif /*MODM_EVALUATORROBUSTNESS_H_*/

