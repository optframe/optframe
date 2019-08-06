#ifndef EFP_CONTRUCTIVE_ACF_HPP_
#define EFP_CONTRUCTIVE_ACF_HPP_

#include <list>
#include <algorithm>
#include <stdlib.h>
#include <set>
//#include "../../OptFrame/Heuristics/GRASP/GRConstructive.hpp"

#include "ProblemInstance.hpp"
#include "Representation.h"
#include "Solution.h"
#include "Evaluator.hpp"
#include "../../OptFrame/Constructive.hpp"

//Tools for ACF
#include "./Util/autocorr.cpp"
#include "./Util/lregress.cpp"

using namespace std;

namespace HFM
{

class ConstructiveACF: public Constructive<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	HFMProblemInstance& pEFP;
	ProblemParameters problemParam;
	RandGen& rg;

	int precisionSP;
	int precisionMP;
	int precisionDP;
	int precision;
	// Your private vars

	vector<double> alphaACF;

public:

	ConstructiveACF(HFMProblemInstance& _pEFP, ProblemParameters& _problemParam, RandGen& _rg, int _precision, vector<double> _alphaACF) : // If necessary, add more parameters
			pEFP(_pEFP), problemParam(_problemParam), rg(_rg), precision(_precision), alphaACF(_alphaACF)
	{

		if (precision == 0)
		{
			cerr << "Precision should be greater than 0 :" << precision << endl;
			getchar();
		}

		//TODO
		//Previously, all of them were receiving precision. Divides equally sounds smarter
		precisionSP = ceil(round(precision / 3));
		precisionMP = ceil(round(precision / 3));
		precisionDP = ceil(round(precision / 3));

//		precisionSP = ceil(round(precision / 2));
//		precisionMP = ceil(round(precision / 2));
//		precisionDP = 1;

		cout << "Inside constructive ACF -- Forcing average and derivative values" << endl;
		precisionSP = precision;
		precisionMP = precision; // 1 because it feeds a random generator -- rg.rand(precisionMP);
		precisionDP = precision;
		precisionMP = 1; // 1 because it feeds a random generator -- rg.rand(precisionMP);
		precisionDP = 1;
	}

	virtual ~ConstructiveACF()
	{
	}

	pair<double, double> returnCorrelationMeanSTD(int input, double limit, int file, int autocor)
	{
		pair<double, double> meanSTD;
		double mean = 0;
		double std = 0;
		int counter = 0;

		if (autocor == 0)
		{
			for (int i = input; i < pEFP.getForecatingsSizeFile(0); i++)
			{
				double value = pEFP.getForecastings(file, i - input);
				if (value >= limit)
				{
					mean += pEFP.getForecastings(0, i);
					counter++;
				}
			}

			if (counter == 0)
			{
				mean = pEFP.getMean(0); //File 0 is the target file
				std = pEFP.getStdDesv(0);
				meanSTD = make_pair(mean, std);
				return meanSTD;
			}

			mean /= counter;

			for (int i = input; i < pEFP.getForecatingsSizeFile(0); i++)
			{
				double value = pEFP.getForecastings(file, i - input);
				if (value >= limit)
				{
					double desv = pEFP.getForecastings(0, i) - mean;
					std += desv * desv;
				}
			}

			std /= counter;
			std = sqrt(std);
		}
		else
		{
			for (int i = input; i < pEFP.getForecatingsSizeFile(0); i++)
			{
				double value = pEFP.getForecastings(file, i - input);
				if (value <= limit)
				{
					mean += pEFP.getForecastings(0, i);
					counter++;
				}
			}

			if (counter == 0)
			{
				mean = pEFP.getMean(0); //File 0 is the target file
				std = pEFP.getStdDesv(0);
				meanSTD = make_pair(mean, std);
				return meanSTD;
			}

			mean /= counter;

			for (int i = input; i < pEFP.getForecatingsSizeFile(0); i++)
			{
				double value = pEFP.getForecastings(file, i - input);
				if (value <= limit)
				{
					double desv = pEFP.getForecastings(0, i) - mean;
					std += desv * desv;

				}
			}

			std /= counter;
			std = sqrt(std);
		}

		meanSTD = make_pair(mean, std);
		return meanSTD;

	}

	Solution<RepHFM>* generateSolution(double timelimit)
	{
		return generateSolutionACF(0.0, timelimit);
	}

	vector<vector<pair<double, int> > > returnRLCUsingACF()
	{
		vector<vector<double> > acfPoints;
		vector<vector<double> > data = pEFP.getForecastingsVector();
		int numberExplanatoryVariables = data.size();

		if ((int) alphaACF.size() != numberExplanatoryVariables)
		{
			cout << "error on limits of ACF builder!" << endl;
//			cout << "forcing some values...." << endl;
//			for (int extraEV = 0; extraEV < (numberExplanatoryVariables - alphaACF.size()); extraEV++)
//				alphaACF.push_back(alphaACF[0]);

			exit(1);
		}

		for (int nEXV = 0; nEXV < numberExplanatoryVariables; nEXV++)
		{
			int nTotalPoints = data[nEXV].size();

			double acfData[nTotalPoints];
			for (int i = 0; i < nTotalPoints; i++)
				acfData[i] = data[nEXV].at(i);

			acorrInfo info;
			autocorr acf(-10, problemParam.getMaxLag(nEXV) + 1); //the ACF generator counts lag 0);
			acf.ACF(acfData, nTotalPoints, info);
			acfPoints.push_back(info.points());
		}

		vector<vector<pair<double, int> > > acfGreedy(numberExplanatoryVariables);

		if (acfPoints[0].size() == 1)
		{
			cout << "ConstructiveACF: exiting Forecasting! There are no points to use as input of the model";
			exit(1);
		}

		for (int nEXV = 0; nEXV < numberExplanatoryVariables; nEXV++)
		{
			while (acfGreedy[nEXV].size() == 0)
			{
				vector<pair<double, int> > acfGreedyPoints;
				for (int k = 0; k < (int) acfPoints[nEXV].size(); k++)
				{
					if ((acfPoints[nEXV][k] >= alphaACF[nEXV]) && (acfPoints[nEXV][k] != 1))
					{
						acfGreedyPoints.push_back(make_pair(acfPoints[nEXV][k], k));
					}
				}
				acfGreedy[nEXV] = acfGreedyPoints;
				if ((acfGreedy[nEXV].size() == 0))
					alphaACF[nEXV] -= 0.01;
			}
		}

//		 cout << alphaACF << endl;
//		 cout << acfPoints << endl;
//		 cout << acfGreedy << endl;
//		 getchar();

		return acfGreedy;
	}

	vector<vector<pair<double, int> > > returnTrivialRLCForSamplesLearning()
	{
		vector<vector<pair<double, int> > > trivialLagsRLC;

		//Jump targetFile
		for (int nEXV = 1; nEXV < pEFP.getNumberExplanatoryVariables(); nEXV++)
		{
			vector<pair<double, int> > vLags;
			vLags.push_back(make_pair(1, 0));
			trivialLagsRLC.push_back(vLags);

		}
		return trivialLagsRLC;
	}

	Solution<RepHFM>* generateSolutionACF(float notUsed, double timelimit)
	{

		//cout << "ACF generating solution.." << endl;
		int numberExplanatoryVariables = pEFP.getNumberExplanatoryVariables();

		vector<vector<pair<double, int> > > lagsRLC = returnRLCUsingACF();

		//Get trivial inputs when learning samples - Time series does not exist
//		if (problemParam.getForceSampleLearningWithEndogenous(0))
//			lagsRLC = returnTrivialRLCForSamplesLearning();

		int earliestInput = 0;
		vector<pair<int, int> > singleIndex;
		vector<vector<double> > singleFuzzyRS;
		vector<vector<pair<int, int> > > averageIndex;
		vector<vector<double> > averageFuzzyRS;
		vector<vector<pair<int, int> > > derivativeIndex;
		vector<vector<double> > derivativeFuzzyRS;

		for (int expVar = 0; expVar < numberExplanatoryVariables; expVar++)
		{
			if(expVar != problemParam.getTargetFile() && problemParam.getForceSampleLearningWithEndogenous(expVar))
				lagsRLC[expVar].push_back(make_pair(1.0,0)); //Add current sample with high ACF TODO

			int nACFUsefullPoints = lagsRLC[expVar].size();
			double mean = pEFP.getMean(expVar);
			double stdDesv = pEFP.getStdDesv(expVar);

			double meanWeight = pEFP.getMean(0); //File 0 is the target file
			double stdDesvWeight = pEFP.getStdDesv(0);

			int pSP = rg.rand(precisionSP);

			for (int p = 0; p < pSP; p++)
			{
				int index = rg.rand(nACFUsefullPoints);
				int K = lagsRLC[expVar][index].second;
				singleIndex.push_back(make_pair(expVar, K));

				if (K > earliestInput)
					earliestInput = K;

				double greater = rg.randG(mean, stdDesv);
				double lower = rg.randG(mean, stdDesv);

				pair<double, double> greaterMeanSTD = returnCorrelationMeanSTD(K, greater, expVar, 0);
				pair<double, double> lowerMeanSTD = returnCorrelationMeanSTD(K, lower, expVar, 1);
				double greaterWeight, lowerWeight;
				greaterWeight = rg.randG(greaterMeanSTD.first, greaterMeanSTD.second);
				lowerWeight = rg.randG(lowerMeanSTD.first, lowerMeanSTD.second);

				//Forcing weights for only changing according to STD TODO
				greaterWeight = rg.randG(greaterMeanSTD.second, greaterMeanSTD.second);
				lowerWeight = rg.randG(lowerMeanSTD.second, lowerMeanSTD.second);

				int fuzzyFunction = rg.rand(N_Activation_Functions);

				vector<double> fuzzyRules;
				fuzzyRules.resize(NCOLUMNATRIBUTES);
				fuzzyRules[GREATER] = greater;
				fuzzyRules[GREATER_WEIGHT] = greaterWeight;
				fuzzyRules[LOWER] = lower;
				fuzzyRules[LOWER_WEIGHT] = lowerWeight;
				fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
				fuzzyRules[PERTINENCEFUNC] = fuzzyFunction; //PERTINENCE FUNCTION

				singleFuzzyRS.push_back(fuzzyRules);
			}
			//cout << nACFUsefullPoints << endl;
			//cout << pSP << endl;
			//cout << singleFuzzyRS << endl;
			//cout << singleIndex << endl;
			//getchar();

			int pMP = rg.rand(precisionMP);

			for (int p = 0; p < pMP; p++)
			{
				int nAveragePoints = rg.rand(5) + 2;
				vector<pair<int, int> > aInputs;
				for (int aI = 0; aI < nAveragePoints; aI++)
				{
					int index = rg.rand(nACFUsefullPoints);
					int K = lagsRLC[expVar][index].second;
					aInputs.push_back(make_pair(expVar, K));
					if (K > earliestInput)
						earliestInput = K;
				}
				averageIndex.push_back(aInputs);

				double greater = rg.randG(mean, stdDesv);
				double lower = rg.randG(mean, stdDesv);
				double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
				double lowerWeight = rg.randG(meanWeight, stdDesvWeight);

				int fuzzyFunction = rg.rand(N_Activation_Functions);

				vector<double> fuzzyRules;
				fuzzyRules.resize(NCOLUMNATRIBUTES);
				fuzzyRules[GREATER] = greater;
				fuzzyRules[GREATER_WEIGHT] = greaterWeight;
				fuzzyRules[LOWER] = lower;
				fuzzyRules[LOWER_WEIGHT] = lowerWeight;
				fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
				fuzzyRules[PERTINENCEFUNC] = fuzzyFunction; //PERTINENCE FUNCTION

				averageFuzzyRS.push_back(fuzzyRules);
			}

			//cout << pMP << endl;
			//cout << averageFuzzyRS << endl;
			//cout << averageIndex << endl;
			//getchar();

			int pDP = rg.rand(precisionDP);
			for (int p = 0; p < pDP; p++)
			{
				int nDerivativePoints = 2;
				vector<pair<int, int> > aInputs;
				for (int aI = 0; aI < nDerivativePoints; aI++)
				{
					int index = rg.rand(nACFUsefullPoints);
					int K = lagsRLC[expVar][index].second;
					aInputs.push_back(make_pair(expVar, K));
					if (K > earliestInput)
						earliestInput = K;
				}
				derivativeIndex.push_back(aInputs);

				double greater = rg.randG(mean, stdDesv);
				double lower = rg.randG(mean, stdDesv);
				double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
				double lowerWeight = rg.randG(meanWeight, stdDesvWeight);

				int fuzzyFunction = rg.rand(N_Activation_Functions);

				vector<double> fuzzyRules;
				fuzzyRules.resize(NCOLUMNATRIBUTES);
				fuzzyRules[GREATER] = greater;
				fuzzyRules[GREATER_WEIGHT] = greaterWeight;
				fuzzyRules[LOWER] = lower;
				fuzzyRules[LOWER_WEIGHT] = lowerWeight;
				fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
				fuzzyRules[PERTINENCEFUNC] = fuzzyFunction; //PERTINENCE FUNCTION

				derivativeFuzzyRS.push_back(fuzzyRules);
			}

			//cout << pDP << endl;
			//cout << derivativeFuzzyRS << endl;
			//cout << derivativeIndex << endl;
			//getchar();
		}

		RepHFM newRep;
		newRep.singleIndex = singleIndex;
		newRep.singleFuzzyRS = singleFuzzyRS;
		newRep.averageIndex = averageIndex;
		newRep.averageFuzzyRS = averageFuzzyRS;
		newRep.derivativeIndex = derivativeIndex;
		newRep.derivativeFuzzyRS = derivativeFuzzyRS;
		newRep.earliestInput = earliestInput;

		//=================================================================
		//When evalAprox,approximationsEnayatifar, are different than 0
		newRep.alpha = rg.rand(100) / 10000.0; //aprox == 1

		//aprox == ( 2 | 4 | 5 )
		vector<double> vAlpha(NAJUSTS, rg.randG(0, 0.1));
		newRep.vAlpha = vAlpha;

		vector<int> vIndex;
		vector<double> vIndexAlphas;

		//aprox == 3
		int nAdjust = rg.rand(1) + 1;
		for (int nA = 0; nA <= nAdjust; nA++)
		{
			//int index = rg.rand(notUsedForTest) + 1;
			int index = rg.rand(lagsRLC[0].size());
			int K = lagsRLC[0][index].second;

			vIndex.push_back(K);
			//double std = rg.rand(300) / 1000;
			vIndexAlphas.push_back(rg.randG(0, 0.1));

		}

		newRep.vIndex = vIndex;
		newRep.vIndexAlphas = vIndexAlphas;
		//=================================================================

		/*
		 cout << newRep.earliestInput << endl;
		 cout << newRep.singleIndex << endl;
		 cout << newRep.singleFuzzyRS << endl;
		 cout << newRep.averageIndex << endl;
		 cout << newRep.averageFuzzyRS << endl;
		 cout << newRep.derivativeIndex << endl;
		 cout << newRep.derivativeFuzzyRS << endl;
		 getchar();
		 */
		//cout << "End of ACF sol generation!" << endl;
		//getchar();
		return new Solution<RepHFM, OPTFRAME_DEFAULT_ADS>(newRep);
	}

};

}

#endif /*EFP_CONTRUCTIVE_ACF_HPP_*/
