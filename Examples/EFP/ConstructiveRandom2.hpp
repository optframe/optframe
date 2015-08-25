#ifndef EFP_CONTRUCTIVE_CALCULATEMEANS_HPP_
#define EFP_CONTRUCTIVE_CALCULATEMEANS_HPP_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>
#include <set>

using namespace std;

namespace EFP
{

class ConstructiveCalcMeans: public Constructive<RepEFP>
{
private:
	ProblemInstance& pEFP;
	RandGen& rg;
	int options;
	int precisionSP;
	int precisionMP;
	int precisionDP;
	int precision;
	// Your private vars
	ProblemParameters problemParam;
	vector<pair<int, int> > oForesSP; //Options to Forecasting with single points
	vector<pair<int, vector<int> > > oForesMP; //Options to Forecasting with mean points
	vector<pair<int, vector<int> > > oForesDP; //Options to Forecasting with derivate points

public:

	ConstructiveCalcMeans(ProblemInstance& _pEFP, ProblemParameters& _problemParam, RandGen& _rg, int _precision) : // If necessary, add more parameters
			pEFP(_pEFP), problemParam(_problemParam), rg(_rg), precision(_precision)
	{
		oForesSP = problemParam.getOForesSP();
		oForesMP = problemParam.getOForesMP();
		oForesDP = problemParam.getOForesDP();

		if (precision == 0)
		{
			cerr << "Precision should be greater than 0 :" << precision << endl;
			getchar();
		}
		precisionSP = precision;
		precisionMP = precision;
		precisionDP = precision;

	}

	virtual ~ConstructiveCalcMeans()
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

	Solution<RepEFP>& generateSolution()
	{
		//cout << "Genereting Random Solution..." << endl;
		RepEFP newRep;
		vector<pair<int, int> > singleIndex;
		vector<vector<double> > singleInputs;
		int earliestInput = 0;

		int sizeSP = oForesSP.size();

		int pSP = rg.rand(precisionSP);

		for (int p = 0; p < pSP; p++)
		{
			int rSP = rg.rand(sizeSP);
			int sFile = oForesSP[rSP].first - 1;
			int sInput = oForesSP[rSP].second;

			if (sInput > earliestInput)
				earliestInput = sInput;

			singleIndex.push_back(make_pair(sFile, sInput));

			double mean = pEFP.getMean(sFile);
			double stdDesv = pEFP.getStdDesv(sFile);

			double greater = rg.randG(mean, stdDesv);
			double lower = rg.randG(mean, stdDesv);

			double meanWeight = pEFP.getMean(0); //File 0 is the target file
			double stdDesvWeight = pEFP.getStdDesv(0);

			pair<double, double> greaterMeanSTD = returnCorrelationMeanSTD(sInput, greater, sFile, 0);
			pair<double, double> lowerMeanSTD = returnCorrelationMeanSTD(sInput, lower, sFile, 1);

			double greaterWeight, lowerWeight;

			greaterWeight = rg.randG(greaterMeanSTD.first, greaterMeanSTD.second);

			lowerWeight = rg.randG(lowerMeanSTD.first, lowerMeanSTD.second);

			/*cout << greaterMeanSTD << endl;
			 cout << lowerMeanSTD << endl;
			 cout << greaterWeight << "\t" << lowerWeight << endl;
			 getchar();*/

			int fuzzyFunction = rg.rand(NFUZZYFUNCTIONS);

			vector<double> fuzzyRules;
			fuzzyRules.resize(NCOLUMNATRIBUTES);
			fuzzyRules[GREATER] = greater;
			fuzzyRules[GREATER_WEIGHT] = greaterWeight;
			fuzzyRules[LOWER] = lower;
			fuzzyRules[LOWER_WEIGHT] = lowerWeight;
			fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
			fuzzyRules[PERTINENCEFUNC] = fuzzyFunction; //PERTINENCE FUNCTION

			singleInputs.push_back(fuzzyRules);
		}

		vector<vector<pair<int, int> > > averageIndex;
		vector<vector<double> > averageInputs;

		int sizeMP = oForesMP.size();

		int pMP = rg.rand(precisionMP);

		for (int p = 0; p < pMP; p++)
		{
			int rMP = rg.rand(sizeMP);

			int aFile = oForesMP[rMP].first - 1;
			vector<int> fileInputs = oForesMP[rMP].second;
			vector<pair<int, int> > aInputs;

			for (int aI = 0; aI < fileInputs.size(); aI++)
			{
				aInputs.push_back(make_pair(aFile, fileInputs[aI]));
				if (fileInputs[aI] > earliestInput)
					earliestInput = fileInputs[aI];
			}

			averageIndex.push_back(aInputs);

			int mean = pEFP.getMean(aFile);
			int stdDesv = pEFP.getStdDesv(aFile);

			double greater = rg.randG(mean, stdDesv);
			double lower = rg.randG(mean, stdDesv);

			double meanWeight = pEFP.getMean(0); //File 0 is the target file
			double stdDesvWeight = pEFP.getStdDesv(0);
			double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
			double lowerWeight = rg.randG(meanWeight, stdDesvWeight);

			int fuzzyFunction = rg.rand(NFUZZYFUNCTIONS);

			vector<double> fuzzyRules;
			fuzzyRules.resize(NCOLUMNATRIBUTES);
			fuzzyRules[GREATER] = greater;
			fuzzyRules[GREATER_WEIGHT] = greaterWeight;
			fuzzyRules[LOWER] = lower;
			fuzzyRules[LOWER_WEIGHT] = lowerWeight;
			fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
			fuzzyRules[PERTINENCEFUNC] = fuzzyFunction; //PERTINENCE FUNCTION

			averageInputs.push_back(fuzzyRules);
		}

		vector<vector<pair<int, int> > > derivativeIndex;
		vector<vector<double> > derivativeInputs;

		int sizeDP = oForesDP.size();
		int pDP = rg.rand(precisionDP);

		for (int p = 0; p < pDP; p++)
		{
			int rDP = rg.rand(sizeDP);
			int dFile = oForesDP[rDP].first - 1;
			vector<int> fileInputs = oForesDP[rDP].second;
			vector<pair<int, int> > dInputs;

			for (int dI = 0; dI < fileInputs.size(); dI++)
			{
				dInputs.push_back(make_pair(dFile, fileInputs[dI]));
				if (fileInputs[dI] > earliestInput)
					earliestInput = fileInputs[dI];
			}

			derivativeIndex.push_back(dInputs);

			int mean = pEFP.getMean(dFile);
			int stdDesv = pEFP.getStdDesv(dFile);

			double greater = 0; //ideal value for derivative, starting in 0 TODO improve it
			double lower = 0; //ideal value for derivative, starting in 0

			double meanWeight = pEFP.getMean(0); //File 0 is the target file
			double stdDesvWeight = pEFP.getStdDesv(0);
			double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
			double lowerWeight = rg.randG(meanWeight, stdDesvWeight);

			int fuzzyFunction = rg.rand(NFUZZYFUNCTIONS);

			vector<double> fuzzyRules;
			fuzzyRules.resize(NCOLUMNATRIBUTES);
			fuzzyRules[GREATER] = greater;
			fuzzyRules[GREATER_WEIGHT] = greaterWeight;
			fuzzyRules[LOWER] = lower;
			fuzzyRules[LOWER_WEIGHT] = lowerWeight;
			fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
			fuzzyRules[PERTINENCEFUNC] = fuzzyFunction; //PERTINENCE FUNCTION

			derivativeInputs.push_back(fuzzyRules);

		}

		/*cout << earliestInput << endl;
		cout << singleIndex << endl;
		cout << singleInputs << endl;
		cout << averageIndex << endl;
		cout << averageInputs << endl;
		cout << derivativeIndex << endl;
		cout << derivativeInputs << endl;
		getchar();*/

		newRep.singleIndex = singleIndex;
		newRep.singleFuzzyRS = singleInputs;
		newRep.averageIndex = averageIndex;
		newRep.averageFuzzyRS = averageInputs;
		newRep.derivativeIndex = derivativeIndex;
		newRep.derivativeFuzzyRS = derivativeInputs;

		newRep.earliestInput = earliestInput;

		vector<double> vAlpha(NAJUSTS, rg.randG(0, 0.1));

		newRep.alpha = rg.rand(100) / 10000.0;
		newRep.vAlpha = vAlpha;

		vector<int> vIndex;
		vector<double> vIndexAlphas;

		int nAdjust = rg.rand(1) + 1;
		for (int nA = 0; nA <= nAdjust; nA++)
		{
			//int index = rg.rand(notUsedForTest) + 1;
			int index = rg.rand(sizeSP);
			index = oForesSP[index].second;

			vIndex.push_back(index);
			double std = rg.rand(300) / 1000;
			vIndexAlphas.push_back(rg.randG(0, 0.1));

		}

		newRep.vIndex = vIndex;
		newRep.vIndexAlphas = vIndexAlphas;


		return *new TestSolution<RepEFP>(newRep);
	}

};

}

#endif /*EFP_CONTRUCTIVE_CALCULATEMEANS_HPP_*/
