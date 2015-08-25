#ifndef EFP_CONTRUCTIVE_RandomNorm_HPP_
#define EFP_CONTRUCTIVE_RandomNorm_HPP_

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

class ConstructiveRandomNorm: public Constructive<RepEFP>
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

	ConstructiveRandomNorm(ProblemInstance& _pEFP, ProblemParameters& _problemParam, RandGen& _rg, int _precision) : // If necessary, add more parameters
			pEFP(_pEFP), problemParam(_problemParam), rg(_rg), precision(_precision)
	{
		oForesSP = problemParam.getOForesSP();
		oForesMP = problemParam.getOForesMP();
		oForesDP = problemParam.getOForesDP();

		precisionSP = precision;
		precisionMP = precision;
		precisionDP = precision;

	}

	virtual ~ConstructiveRandomNorm()
	{
	}

	Solution<RepEFP>& generateSolution()
	{
		//cout << "Genereting Random Solution..." << endl;
		RepEFP newRep;
		vector<pair<int, int> > singleIndex;
		vector<vector<double> > singleInputs;
		int earliestInput = 0;

		int sizeSP = oForesSP.size();

		for (int p = 0; p < precisionSP; p++)
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

			double greaterWeight = rg.randG(meanWeight, stdDesvWeight);
			double lowerWeight = rg.randG(meanWeight, stdDesvWeight);

			vector<double> fuzzyRules;
			fuzzyRules.resize(NCOLUMNATRIBUTES);
			fuzzyRules[GREATER] = greater;
			fuzzyRules[GREATER_WEIGHT] = greaterWeight;
			fuzzyRules[LOWER] = lower;
			fuzzyRules[LOWER_WEIGHT] = lowerWeight;
			fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
			fuzzyRules[PERTINENCEFUNC] = 0; //PERTINENCE FUNCTION

			singleInputs.push_back(fuzzyRules);
		}

		vector<vector<pair<int, int> > > averageIndex;
		vector<vector<double> > averageInputs;

		int sizeMP = oForesMP.size();

		for (int p = 0; p < precisionMP; p++)
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

			vector<double> fuzzyRules;
			fuzzyRules.resize(NCOLUMNATRIBUTES);
			fuzzyRules[GREATER] = greater;
			fuzzyRules[GREATER_WEIGHT] = greaterWeight;
			fuzzyRules[LOWER] = lower;
			fuzzyRules[LOWER_WEIGHT] = lowerWeight;
			fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
			fuzzyRules[PERTINENCEFUNC] = 0; //PERTINENCE FUNCTION

			averageInputs.push_back(fuzzyRules);
		}

		vector<vector<pair<int, int> > > derivativeIndex;
		vector<vector<double> > derivativeInputs;

		int sizeDP = oForesDP.size();
		for (int p = 0; p < precisionDP; p++)
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

			vector<double> fuzzyRules;
			fuzzyRules.resize(NCOLUMNATRIBUTES);
			fuzzyRules[GREATER] = greater;
			fuzzyRules[GREATER_WEIGHT] = greaterWeight;
			fuzzyRules[LOWER] = lower;
			fuzzyRules[LOWER_WEIGHT] = lowerWeight;
			fuzzyRules[EPSILON] = 1; //TODO TEST FOR TRAPEZOID
			fuzzyRules[PERTINENCEFUNC] = 0; //PERTINENCE FUNCTION

			derivativeInputs.push_back(fuzzyRules);

		}

		/*
		 cout<<singleIndex<<endl;
		 cout<<singleInputs<<endl;
		 cout<<averageIndex<<endl;
		 cout<<averageInputs<<endl;
		 cout<<derivativeIndex<<endl;
		 cout<<derivativeInputs<<endl;
		 getchar();
		 */

		newRep.singleIndex = singleIndex;
		newRep.singleFuzzyRS = singleInputs;
		newRep.averageIndex = averageIndex;
		newRep.averageFuzzyRS = averageInputs;
		newRep.derivativeIndex = derivativeIndex;
		newRep.derivativeFuzzyRS = derivativeInputs;

		newRep.earliestInput = earliestInput;

		vector<double> vAlpha(NAJUSTS, rg.rand(100) / 10000.0);

		newRep.alpha = rg.rand(100) / 10000.0;
		newRep.vAlpha = vAlpha;

		vector<int> vIndex;
		vector<double> vIndexAlphas;

		int nAdjust = rg.rand(1) + 1;
		for (int nA = 0; nA <= nAdjust; nA++)
		{
			int index = rg.rand(newRep.earliestInput) + 1;
			vIndex.push_back(index);
			vIndexAlphas.push_back(rg.rand(100) / 10000.0);
		}

		newRep.vIndex = vIndex;
		newRep.vIndexAlphas = vIndexAlphas;

		return *new TestSolution<RepEFP>(newRep);
	}

};

}

#endif /*EFP_CONTRUCTIVE_RandomNorm_HPP_*/
