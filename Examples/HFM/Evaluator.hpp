#ifndef HFM_EVALUATOR_HPP_
#define HFM_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>
#include <utility>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"
#include <algorithm>
#include "TreatForecasts.hpp"

extern int nThreads;

using namespace std;

namespace HFM
{

enum PerformanceIndicator
{
	MAPE_INDEX, MAE_INDEX, MSE_INDEX, RMSE_INDEX, MAPE_INV_INDEX, SMAPE_INDEX, MMAPE_INDEX, WMAPE_INDEX, PINBALL_INDEX, PINBALL_ERROR_INDEX, EVALUTORS_NMETRICS_ENUM_COUNT, ALL_EVALUATIONS
};

class HFMEvaluator: public Evaluator<RepHFM, OPTFRAME_DEFAULT_ADS>
{
private:
	HFMProblemInstance& pEFP;
	ProblemParameters& problemParam;

	//int optionsP;
	int nForecastings;

	//desired optmization metric
	int optMetric;
	int aprox;
	int nEvalForSpeedUp;

	double timeInMS;

	double timeTest, timeTest2;
	int counter;
	int targetFile;
public:

	HFMEvaluator(HFMProblemInstance& _pEFP, ProblemParameters& _problemParam, int _optMetric, int _aprox) : // If necessary, add more parameters
			pEFP(_pEFP), problemParam(_problemParam), optMetric(_optMetric), aprox(_aprox)
	{
		nForecastings = pEFP.getForecatingsSizeFile(0); //all files have the same size
		targetFile = problemParam.getTargetFile();

		nEvalForSpeedUp = 0;
		timeInMS = 0;
		timeTest = 0;
		timeTest2 = 0;
		counter = 0;
	}

	virtual ~HFMEvaluator()
	{

	}

	EvaluationEFP evaluate(const RepHFM& rep, const OPTFRAME_DEFAULT_ADS*)
	{
		//Fo vector with different metrics calculations
		vector<double>* foIndicator = evaluateAll(rep, optMetric);

		double fo = foIndicator->at(optMetric);		// Evaluation Function Value
		delete foIndicator;
		return EvaluationEFP(fo);
	}

	vector<double>* evaluateAll(const RepHFM& rep, const int accIndicator, vector<vector<double> >* vForecastings = nullptr)
	{
		if (vForecastings == nullptr)
			vForecastings = &pEFP.getForecastingsVector();

		pair<vector<double>*, vector<double>*>* targetAndForecasts = generateSWMultiRoundForecasts(rep, *vForecastings, problemParam.getStepsAhead());
		vector<double>* foIndicator = getAccuracy(*targetAndForecasts->first, *targetAndForecasts->second, accIndicator);

		delete targetAndForecasts->first;
		delete targetAndForecasts->second;
		delete targetAndForecasts;

		return foIndicator;
	}

	vector<double>* returnForecasts(const RepHFM& rep, const vector<vector<double> >& vForecastings, const int begin, const int fh)
	{
		int sizeSP = rep.singleIndex.size();
		int sizeAP = rep.averageIndex.size();
		int sizeDP = rep.derivativeIndex.size();

		vector<double>* predicteds = new vector<double>(fh);

		//for (int pa = 0; ((pa < stepsAhead) && (pa + begin < nForTargetFile)); pa++) //auxiliar loop for steps ahead
		for (int pa = 0; pa < fh; pa++)	 // passos a frente
		{
			double estimation = 0;
			int accepted = 0;

			for (int nSP = 0; nSP < sizeSP; nSP++)
			{
				int file = rep.singleIndex[nSP].first;
				int K = rep.singleIndex[nSP].second;

				double singleValue = getKValue(K, file, begin, pa, vForecastings, *predicteds);

				double ruleGreater = rep.singleFuzzyRS[nSP][GREATER];
				double greaterWeight = rep.singleFuzzyRS[nSP][GREATER_WEIGHT];
				double ruleLower = rep.singleFuzzyRS[nSP][LOWER];
				double lowerWeight = rep.singleFuzzyRS[nSP][LOWER_WEIGHT];
				double ruleEpsilon = rep.singleFuzzyRS[nSP][EPSILON];
				ActivationFunction repFuzzyPertinenceFunc = ActivationFunction(rep.singleFuzzyRS[nSP][PERTINENCEFUNC]);

				defuzzification(ruleGreater, greaterWeight, ruleLower, lowerWeight, ruleEpsilon, repFuzzyPertinenceFunc, singleValue, estimation, accepted);
			}

			for (int nMP = 0; nMP < sizeAP; nMP++)
			{
				vector<pair<int, int> > meansK = rep.averageIndex[nMP];
				int nAveragePoints = meansK.size();

				double mean = 0;
				for (int mK = 0; mK < nAveragePoints; mK++)
				{
					int file = meansK[mK].first;
					int K = meansK[mK].second;

					mean += getKValue(K, file, begin, pa, vForecastings, *predicteds);
				}
				mean = mean / nAveragePoints;

				double ruleGreater = rep.averageFuzzyRS[nMP][GREATER];
				double greaterWeight = rep.averageFuzzyRS[nMP][GREATER_WEIGHT];
				double ruleLower = rep.averageFuzzyRS[nMP][LOWER];
				double lowerWeight = rep.averageFuzzyRS[nMP][LOWER_WEIGHT];
				double ruleEpsilon = rep.averageFuzzyRS[nMP][EPSILON];
				ActivationFunction repFuzzyPertinenceFunc = ActivationFunction(rep.averageFuzzyRS[nMP][PERTINENCEFUNC]);

				defuzzification(ruleGreater, greaterWeight, ruleLower, lowerWeight, ruleEpsilon, repFuzzyPertinenceFunc, mean, estimation, accepted);
			}

			for (int nDP = 0; nDP < sizeDP; nDP++)
			{
				vector<pair<int, int> > derivateK = rep.derivativeIndex[nDP];

				double d = 0;
				for (int dK = 0; dK < (int) derivateK.size(); dK++)
				{
					int file = derivateK[dK].first;
					int K = derivateK[dK].second;

					double value = getKValue(K, file, begin, pa, vForecastings, *predicteds);

					if (dK == 0)
						d += value;
					else
						d -= value;
				}

				double ruleGreater = rep.derivativeFuzzyRS[nDP][GREATER];
				double greaterWeight = rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT];
				double ruleLower = rep.derivativeFuzzyRS[nDP][LOWER];
				double lowerWeight = rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT];
				double ruleEpsilon = rep.derivativeFuzzyRS[nDP][EPSILON];
				ActivationFunction repFuzzyPertinenceFunc = ActivationFunction(rep.derivativeFuzzyRS[nDP][PERTINENCEFUNC]);

				defuzzification(ruleGreater, greaterWeight, ruleLower, lowerWeight, ruleEpsilon, repFuzzyPertinenceFunc, d, estimation, accepted);
			}

			//Applying rules weights
			if (accepted > 0)
				estimation /= accepted;

			//Approximations using time series backshift operators
			if (aprox != 0)
				approximationsEnayatifar(aprox, rep.alpha, rep.vAlpha, rep.vIndexAlphas, rep.vIndex, estimation, begin, pa, vForecastings, *predicteds);

			//================================================
			//Apply special operators, rounding, binary, on estimated value
			modifyEstimation(estimation);
			//================================================
			predicteds->at(pa) = estimation;

		} //Forecasting horizon loop -- Steps Ahead

		return predicteds;
	}

	double getKValue(const int K, const int expVariable, const int begin, const int pa, const vector<vector<double> >& matrixWithSamples, const vector<double>& predicteds)
	{
		double value = 0;
		int nSamplesVar = matrixWithSamples.at(expVariable).size();

		if (((begin + pa - K) < 0))
		{
			cout << "BUG Evaluator (function getKValue): (i + pa - K) = " << begin + pa - K << endl;
			cout << matrixWithSamples.size() << endl;
			cout << "vForecastings[" << expVariable << "].size() = " << nSamplesVar << endl;
			assert((begin + pa - K) < 0);
		}

		//Curent sample mode might be on
		if ((K == 0))
		{
			//This can never happen - Current Sample from targetFile can never be used
			assert(expVariable != targetFile);

			if (!problemParam.getForceSampleLearningWithEndogenous(expVariable))
			{
				cout << "Wrong Lag request (function getKValue): K = " << K << " expVar = " << expVariable << endl;
				cout << "Current sample learning is not allowed for this Endogenous variable!" << endl;
				exit(1);
			}
			else
			{
				if ((begin + pa) < nSamplesVar)
					value = matrixWithSamples.at(expVariable).at(begin + pa - K);
				else
					value = 0; //because we may not have samples at some specific parts of the historical data

//				cout << "Go Ahead! Sample learning mode on! Current sample value = " << value<< "\texpVar = " << expVariable << endl;
//				getchar();
				return value;
			}
		}

		if ((pa >= K) && (expVariable == targetFile) && (K > 0))
		{
			value = predicteds.at(pa - K);
		}
		else
		{
			if ((begin + pa - K) < nSamplesVar)
				value = matrixWithSamples.at(expVariable).at(begin + pa - K);
			else
				value = 0;
		}

		return value;
	}

//defuzzification process -- Converter a node input into useful estimation values
	void defuzzification(const double ruleGreater, const double greaterWeight, const double ruleLower, const double lowerWeight, double ruleEpsilon, const ActivationFunction fuzzyFunc, const double value, double& estimation, int& accepted)
	{

		if (fuzzyFunc == Heaviside)
		{
			if (value >= ruleGreater)
			{
				estimation += (greaterWeight + value);
				accepted++;
			}

			if (value <= ruleLower)
			{
				estimation += (lowerWeight + value);
				accepted++;
			}

		}

		double epsilon = ruleEpsilon;
		//Trapezoid Function
		if (fuzzyFunc == Trapezoid)
		{
			double a = ruleGreater;
			double mu = 0;
			if (value <= (a - epsilon))
				mu = 0;
			if (value > a)
				mu = 1;
			if ((value > (a - epsilon)) && value <= a)
			{
				double K1 = 1 / epsilon;
				double K2 = 1 - a * K1;
				mu = value * K1 + K2;
			}

			estimation += (greaterWeight * mu) + value;
			accepted++;
		}

		if (fuzzyFunc == Trapezoid)
		{
			//TODO Implement Sigmoid
			/*** Exponential calculation ***/
//			exp_value = exp((double) -x);
			/*** Final sigmoid value ***/
//			return_value = 1 / (1 + exp_value);
			double b = ruleLower;
			double mu = 0;
			if (value >= (b + epsilon))
				mu = 0;
			if (value < b)
				mu = 1;
			if (value >= b && value < b + epsilon)
			{
				double K1 = 1 / epsilon;
				double K2 = 1 - b * K1;
				mu = value * K1 + K2;
			}

			estimation += (lowerWeight * mu) + value;
			accepted++;
		}

	}

//Different approximations that can be used to approximate a forecasts with pre-defined rules (some can be consider optimized weights)
	void approximationsEnayatifar(const int& aprox, const double& alpha, const vector<double>& vAlpha, const vector<double>& vIndexAlphas, const vector<int>& vIndex, double& estimation, const int i, const int pa, const vector<vector<double> >& vForecastings, const vector<double>& predicteds)
	{
		if (aprox == 1)
		{
			double ajusts = 0;
			vector<int> adjustPrevValue;
			adjustPrevValue.push_back(1);
			adjustPrevValue.push_back(2);
			adjustPrevValue.push_back(24);
			adjustPrevValue.push_back(25);
			adjustPrevValue.push_back(167);
			adjustPrevValue.push_back(168);
			adjustPrevValue.push_back(169);

			for (int ind = 0; ind < int(adjustPrevValue.size()); ind++)
			{
				int indexK = adjustPrevValue[ind];
				double valueIndexK = 0;
				if (indexK <= i)
				{
					if (pa >= indexK)
						valueIndexK += predicteds[pa - indexK];
					else
						valueIndexK += vForecastings[targetFile][i + pa - indexK];

					ajusts += alpha * (estimation - valueIndexK);
				}
			}
			estimation = estimation + ajusts;
		}

		if (aprox == 2)
		{
			double ajusts = 0;
			vector<int> adjustPrevValue;
			adjustPrevValue.push_back(1);
			adjustPrevValue.push_back(2);
			adjustPrevValue.push_back(3);
			adjustPrevValue.push_back(23);
			adjustPrevValue.push_back(24);
			adjustPrevValue.push_back(25);
			for (int ind = 0; ind < int(adjustPrevValue.size()); ind++)
			{
				int indexK = adjustPrevValue[ind];
				double valueIndexK = 0;
				if (indexK <= (i + pa))
				{
					if (pa >= indexK)
						valueIndexK += predicteds[pa - indexK];
					else
					{
						valueIndexK += vForecastings[targetFile][i + pa - indexK];
					}
					ajusts += vAlpha[ind] * (estimation - valueIndexK);
				}

			}

			estimation = estimation + ajusts;

		}

		if (aprox == 3)
		{
			double ajusts = 0;

			for (int ind = 0; ind < int(vIndex.size()); ind++)
			{
				int indexK = vIndex[ind];
				double valueIndexK = 0;
				if (indexK <= i)
				{
					if (pa >= indexK)
						valueIndexK += predicteds[pa - indexK];
					else
						valueIndexK += vForecastings[targetFile][i + pa - indexK];

					int maxLag = problemParam.getMaxLag(targetFile);
					ajusts += vIndexAlphas[ind] * (estimation - valueIndexK);
					assert(vIndex[ind] > 0);
					assert(vIndex[ind] <= maxLag);
//					if ((vIndex[ind] <= 0) || (vIndex[ind] > maxLag))
//					{
//						cout << "vIndex[ind]:" << vIndex[ind] << "maxLag:" << maxLag << endl;
//						cout << "Erro on INDEX of the adjustment";
//						getchar();
//					}
				}

			}

			estimation = estimation + ajusts;
		}

		if (aprox == 4)
		{
			double ajusts = 0;
			vector<int> adjustPrevValue;
			adjustPrevValue.push_back(10);
			adjustPrevValue.push_back(50);
			adjustPrevValue.push_back(15);
			adjustPrevValue.push_back(25);
			adjustPrevValue.push_back(38);
			adjustPrevValue.push_back(100);

			for (int ind = 0; ind < int(adjustPrevValue.size()); ind++)
			{

				int indexK = adjustPrevValue[ind];
				double valueIndexK = 0;
				if (indexK <= (i + pa))
				{
					if (pa >= indexK)
						valueIndexK += predicteds[pa - indexK];
					else
						valueIndexK += vForecastings[targetFile][i + pa - indexK];

					ajusts += vAlpha[ind] * (estimation - valueIndexK);
				}
			}

			estimation = estimation + ajusts;

		}

		if (aprox == 5)
		{
			double ajusts = 0;
			vector<int> adjustPrevValue;
			adjustPrevValue.push_back(1);
			adjustPrevValue.push_back(24);
			adjustPrevValue.push_back(48);
			adjustPrevValue.push_back(168);
			adjustPrevValue.push_back(168 * 2);
			adjustPrevValue.push_back(168 * 3);
			for (int ind = 0; ind < int(adjustPrevValue.size()); ind++)
			{
				int indexK = adjustPrevValue[ind];
				double valueIndexK = 0;
				if (indexK <= (i + pa))
				{
					if (pa >= indexK)
						valueIndexK += predicteds[pa - indexK];
					else
					{
						valueIndexK += vForecastings[targetFile][i + pa - indexK];
					}
					ajusts += vAlpha[ind] * (estimation - valueIndexK);
				}

			}

			estimation = estimation + ajusts;

		}
	}

	void modifyEstimation(double& estimation)
	{
		//Block all negative predicted values
		if (problemParam.getNonNegativeForecastsStatus())
		{
			if (estimation < 0)
				estimation = 0;
		}

		//Do not allow real values, round all values to integer
		if (problemParam.getRoundedForecastsStatus())
		{
			estimation = round(estimation);
		}

		//Rounding target to binary output
		if (problemParam.getBinaryForecastsStatus())
		{
			estimation = round(estimation);
			if (estimation < 0)
				estimation = 0;

			if (estimation > 1)
				estimation = 1;
		}
	}

	//return a vector with size equal to the number of indicators -- However, only accIndicator index is calculated
	//if equals to -1 it calls all indicators calculus
	vector<double>* getAccuracy(const vector<double>& targetValues, const vector<double>& estimatedValues, const int accIndicator)
	{

		int nSamples = targetValues.size();
		if (nSamples != (int) estimatedValues.size())
		{
			cout << "ERROR on GetAccuracy! Different sizes!" << endl;
			cout << "targetValues.size() = " << targetValues.size() << "\t";
			cout << "estimatedValues.size() = " << estimatedValues.size() << endl;
		}
		vector<double>* foIndicator = new vector<double>(EVALUTORS_NMETRICS_ENUM_COUNT, 0);

		double sumTarget = 0;
		double avgTarget = 0;
		if (accIndicator == WMAPE_INDEX || accIndicator == MMAPE_INDEX || accIndicator == ALL_EVALUATIONS)
		{
			for (int i = 0; i < nSamples; i++)
				sumTarget += targetValues[i];

			avgTarget = sumTarget / nSamples;
		}

		for (int i = 0; i < nSamples; i++)
		{
			double estimation = estimatedValues[i];
			double forecastingTarget = targetValues[i];
			double forecastingTargetNotNull = forecastingTarget;
			if (forecastingTargetNotNull == 0)
				forecastingTargetNotNull = 0.0001;

			double absDiff = abs(estimation - forecastingTarget);

			if (accIndicator == MAPE_INDEX || accIndicator == ALL_EVALUATIONS)
				foIndicator->at(MAPE_INDEX) += (absDiff / abs(forecastingTargetNotNull));

			if (accIndicator == MAPE_INV_INDEX || accIndicator == ALL_EVALUATIONS)
			{
				double forecastingTargetNotMax = forecastingTarget;
				double tsMaxValues = pEFP.getMax(targetFile);
				if (forecastingTargetNotMax == tsMaxValues)
					forecastingTargetNotMax = tsMaxValues - 0.0001;
				foIndicator->at(MAPE_INV_INDEX) += (absDiff / abs(tsMaxValues - forecastingTargetNotMax));
			}

			if (accIndicator == MAE_INDEX || accIndicator == ALL_EVALUATIONS)
				foIndicator->at(MAE_INDEX) += absDiff;

			if (accIndicator == MSE_INDEX || accIndicator == RMSE_INDEX || accIndicator == ALL_EVALUATIONS)
				foIndicator->at(MSE_INDEX) += pow(absDiff, 2);

			if (accIndicator == SMAPE_INDEX || accIndicator == ALL_EVALUATIONS)
			{
				double forecastingTargetNotNullSMAPE = abs(forecastingTarget + estimation);
				if (forecastingTargetNotNullSMAPE == 0)
					forecastingTargetNotNullSMAPE = 0.0001;

				foIndicator->at(SMAPE_INDEX) += (absDiff / forecastingTargetNotNullSMAPE / 2);
			}

			if (accIndicator == WMAPE_INDEX || accIndicator == ALL_EVALUATIONS)
				foIndicator->at(WMAPE_INDEX) += (absDiff / sumTarget);

			if (accIndicator == MMAPE_INDEX || accIndicator == ALL_EVALUATIONS)
				foIndicator->at(MMAPE_INDEX) += (absDiff / avgTarget);

			//PinballFunction
			if (accIndicator == PINBALL_INDEX || accIndicator == ALL_EVALUATIONS)
			{
				double quantilError = 0.5;
				double pinballFunctionQuantils = 0;
				for (int a = 1; a < 100; a++)
				{
					double quantilFactor = (50 - a) / 100.0;

					double qa = estimation - estimation * quantilError * quantilFactor;

					if (forecastingTarget < qa)
						pinballFunctionQuantils += (1 - (a / 100.0)) * (qa - forecastingTarget);

					if (forecastingTarget >= qa)
						pinballFunctionQuantils += (a / 100.0) * (forecastingTarget - qa);
				}
				pinballFunctionQuantils /= 99;
				foIndicator->at(PINBALL_INDEX) += pinballFunctionQuantils;
			}

		}

		//MAPE FINAL CALC
		if (accIndicator == MAPE_INDEX || accIndicator == ALL_EVALUATIONS)
		{
			foIndicator->at(MAPE_INDEX) *= 100;
			foIndicator->at(MAPE_INDEX) /= nSamples;
		}

		if (accIndicator == MAPE_INV_INDEX || accIndicator == ALL_EVALUATIONS)
		{
			foIndicator->at(MAPE_INV_INDEX) *= 100;
			foIndicator->at(MAPE_INV_INDEX) /= nSamples;
		}

		if (accIndicator == SMAPE_INDEX || accIndicator == ALL_EVALUATIONS)
		{
			foIndicator->at(SMAPE_INDEX) *= 100;
			foIndicator->at(SMAPE_INDEX) /= nSamples;
		}

		if (accIndicator == WMAPE_INDEX || accIndicator == ALL_EVALUATIONS)
		{
			foIndicator->at(WMAPE_INDEX) *= 100;
			foIndicator->at(WMAPE_INDEX) /= nSamples;
		}

		if (accIndicator == MMAPE_INDEX || accIndicator == ALL_EVALUATIONS)
		{
			foIndicator->at(MMAPE_INDEX) *= 100;
			foIndicator->at(MMAPE_INDEX) /= nSamples;
		}

		if (accIndicator == PINBALL_INDEX || accIndicator == ALL_EVALUATIONS)
			foIndicator->at(PINBALL_INDEX) /= nSamples;

		if (accIndicator == MAE_INDEX || accIndicator == ALL_EVALUATIONS)
			foIndicator->at(MAE_INDEX) /= nSamples;

		if (accIndicator == MSE_INDEX || accIndicator == RMSE_INDEX || accIndicator == ALL_EVALUATIONS)
		{
			foIndicator->at(MSE_INDEX) /= nSamples;

			if (accIndicator == RMSE_INDEX || accIndicator == -1)
				foIndicator->at(RMSE_INDEX) = sqrt(foIndicator->at(MSE_INDEX));
		}

		// ===================  MAPE FOR EACH STEP AHEAD ========================
		/*for (int mp = 0; mp < stepsAhead; mp++)
		 {
		 foIndicatorStepsAhead[MAPE_INDEX][mp] = foIndicatorStepsAhead[0][mp] * 100;

		 foIndicatorStepsAhead[MAPE_INDEX][mp] = foIndicatorStepsAhead[0][mp] / ((nForecastingValidations - notUsedForTest) / stepsAhead);
		 }
		 */
		// ===================  MAPE FOR EACH STEP AHEAD ========================
		//cout << foIndicator[MAPE_INDEX] << endl;
		//MSE AND RSME FINAL CALC
		//PINBALL FINAL CALC
		//		vector<pair<double, double> > vPinballFunctions(20, make_pair(0, 0));
		/*
		 double minPinball = 1000000;
		 double pinbalError;
		 for (int pF = 0; pF < 20; pF++)
		 {
		 vPinballFunctions[pF].first /= (nForTargetFile - earliestInput);
		 if (vPinballFunctions[pF].first < minPinball)
		 {
		 minPinball = vPinballFunctions[pF].first;
		 pinbalError = vPinballFunctions[pF].second;
		 }
		 }

		 foIndicator[PINBALL_INDEX] = minPinball;
		 foIndicator[PINBALL_ERROR_INDEX] = pinbalError;*/

		//cout << "Fo Indicator:";
		//cout << foIndicator << endl;
		//cout << "steps Ahead:";
		//cout << foIndicatorStepsAhead << endl;
		return foIndicator;
	}

//return a pair -- first vector contain the real values while the second has the forecasted ones - StepsSize=slidingWindowStepSize
	//Sliding Window Multi Round Forecasts with stepSize
	pair<vector<double>*, vector<double>*>* generateSWMultiRoundForecasts(const RepHFM& rep, const vector<vector<double> >& vForecastings, const int slidingWindowStep)
	{
		vector<double>* allForecasts = new vector<double>;
		vector<double>* allTargets = new vector<double>;

		int nForTargetFile = vForecastings[targetFile].size();
		int maxLag = problemParam.getMaxLag(targetFile);
		int nSamples = nForTargetFile - maxLag;

		int beginParallel;

		if (nThreads > 1)
		{
			//===================================================================
			//		PARALELL CODE FOR MULTI CPU
			vector<vector<double> > allForecastsVectors(ceil(double(nSamples) / slidingWindowStep));
			vector<vector<double> > allForecastsTarget(ceil(double(nSamples) / slidingWindowStep));

#pragma omp parallel for num_threads(nThreads)
			for (beginParallel = maxLag; beginParallel < nForTargetFile; beginParallel += slidingWindowStep) // main loop that varries all the time series
			{
				vector<double>* forecasts = returnForecasts(rep, vForecastings, beginParallel, problemParam.getStepsAhead());

				int fhOrTsSize = std::min(slidingWindowStep, nForTargetFile - beginParallel);
				int index = (beginParallel - maxLag) / slidingWindowStep;

				allForecastsVectors[index].insert(allForecastsVectors[index].end(), forecasts->begin(), forecasts->begin() + fhOrTsSize);
				allForecastsTarget[index].insert(allForecastsTarget[index].end(), vForecastings[targetFile].begin() + beginParallel, vForecastings[targetFile].begin() + beginParallel + fhOrTsSize);
				delete forecasts;
			}

			for (int aV = 0; aV < (int) allForecastsVectors.size(); aV++)
				for (int k = 0; k < (int) allForecastsVectors[aV].size(); k++)
				{
					allForecasts->push_back(allForecastsVectors[aV][k]);
					allTargets->push_back(allForecastsTarget[aV][k]);
				}
			//		PARALELL CODE FOR MULTI CPU
			//===================================================================
		}
		else
		{

			for (beginParallel = maxLag; beginParallel < nForTargetFile; beginParallel += slidingWindowStep) // main loop that varries all the time series
			{
				vector<double>* forecasts = returnForecasts(rep, vForecastings, beginParallel, problemParam.getStepsAhead());
				int fhOrTsSize = std::min(slidingWindowStep, nForTargetFile - beginParallel);

				for (int i = 0; i < fhOrTsSize; i++)
				{
					allForecasts->push_back(forecasts->at(i));
					allTargets->push_back(vForecastings[targetFile][beginParallel + i]);
				}

//				allForecasts->insert(allForecasts->end(), forecasts->begin(), forecasts->begin() + fhSize);
//				allTargets->insert(allTargets->end(), vForecastings[targetFile].begin() + beginParallel, vForecastings[targetFile].begin() + beginParallel + fhSize);
				delete forecasts;
			}
		}

		return new pair<vector<double>*, vector<double>*>(make_pair(allTargets, allForecasts));
	}

	virtual bool isMinimization() const
	{
		return true;
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		return "OptFrame:Evaluator:HFMEvaluator";
	}

//	virtual bool betterThan(double a, double b)
//	{
//		// MINIMIZATION
//		return (a < (b - EPSILON_EFP));
//	}

//
//	vector<double> evaluateAll(const RepEFP& rep, int accIndicator)
//	{
//		Timer tnow;
//		//Fo vector with different metrics calculations
//		vector<double> foIndicator(NMETRICS + 1, 0); //nMetrics + ErrorPinball
//
//		/*
//		 vector<vector<double> > foIndicatorStepsAhead(NMETRICS + 1);
//		 for (int i = 0; i < foIndicatorStepsAhead.size(); i++)
//		 foIndicatorStepsAhead[i].resize(stepsAhead);*/
//
//		vector<vector<double> >& vForecastings = pEFP.getForecastingsVector();
//
//		//validation mode = false | returnforecasts = false
//		vector<double> predictedValues = generateSWMultiRoundForecasts(rep, vForecastings);
//
//		int maxLag = problemParam.getMaxLag();
//		vector<double> targetValues;
//		for (int i = maxLag; i < vForecastings[targetFile].size(); i++)
//			targetValues.push_back(vForecastings[targetFile][i]);
//
//		foIndicator = getAccuracy(targetValues, predictedValues, accIndicator);
//
//		timeTest+=tnow.now();
//		Timer tNow2;
//		vector<double> foIndicator2 = evaluateAll2(rep, accIndicator);
//		timeTest2+=tNow2.now();
//		counter++;
//		if(counter%10000==0)
//		{
//			cout<<"("<<timeTest/counter<<","<<timeTest2/counter<<")\n";
//			counter=0;
//			timeTest=0;
//			timeTest2=0;
//		}
//		if (foIndicator2 != foIndicator)
//		{
//			cout << foIndicator2 << endl;
//			cout << foIndicator << endl;
//			getchar();
//		}
//
//		return foIndicator;
//	}

//	//generate sliding window, with FH steps, multi round forecasts
//	vector<double> generateSWMultiRoundForecasts(const RepEFP& rep, const vector<vector<double> >& vForecastings)
//	{
//		int nForTargetFile = vForecastings[targetFile].size();
//		int maxLag = problemParam.getMaxLag();
//		int nSamples = nForTargetFile - maxLag;
//
//		vector<double> allForecasts;
//
//		double effectNumberSamples = (nForTargetFile - maxLag);
//		vector<vector<double> > allForecastsVectors(ceil(effectNumberSamples / stepsAhead));
//
//		//Timer t;
//
//		//for (int begin = maxLag; begin < nForTargetFile; begin ++) // main loop that varries all the time series
//		int beginParallel, fParallel;
////		omp_set_num_threads(4);
//		//			int np = omp_get_num_threads();
//		//					cout << "number of threads: " << np << endl;
//		//							getchar();
//
//////		#pragma omp parallel for ordered
////		for (beginParallel = maxLag; beginParallel < nForTargetFile; beginParallel += stepsAhead) // main loop that varries all the time series
////		{
//////			omp_set_dynamic(0);     // Explicitly disable dynamic teams
//////			omp_set_num_threads(4); // Use 4 threads for all consecutive parallel regions
////
////			vector<double> predicteds = returnForecasts(rep, vForecastings, beginParallel);
////
//////			#pragma omp ordered
////			allForecasts.insert(allForecasts.end(), predicteds.begin(), predicteds.end());
////
////
////
//////			for (fParallel = 0; fParallel < predicteds.size(); fParallel++)
//////			{
//////				allForecasts.push_back(predicteds[fParallel]);
//////			}
////		}
//
////		cout<<"nThreads:"<<nThreads<<endl;
////		getchar();
//
//		//#pragma omp parallel for num_threads(nThreads)
//		for (beginParallel = maxLag; beginParallel < nForTargetFile; beginParallel += stepsAhead) // main loop that varries all the time series
//		{
//			int index = (beginParallel - maxLag) / stepsAhead;
//
//			allForecastsVectors[index] = std::move(returnForecasts(rep, vForecastings, beginParallel));			//predicteds;
//		}
//
//		for (int aV = 0; aV < allForecastsVectors.size(); aV++)
//			for (int k = 0; k < stepsAhead; k++)
//				allForecasts.push_back(allForecastsVectors[aV][k]);
//
//		//TODO - Parallel Test and report values
//
////
////		 timeInMS += t.inMilliSecs();
////		 nEvalForSpeedUp++;
////		 if (nEvalForSpeedUp == 1000)
////		 {
////		 cout << "Total time: " << timeInMS / 1000 << endl;
////
////		 string speedUpFile = "./apen_SI_speedUpFile";
////		 FILE* fResults = fopen(speedUpFile.c_str(), "a");
////		 fprintf(fResults, "%.3f\n", timeInMS / 1000);
////		 fclose(fResults);
////
////		 exit(1);
////		 }
//
//
//		//TODO do it in a better style
//		if (allForecasts.size() > nSamples)
//		{
//			int nExtraForecasts = allForecasts.size() - nSamples;
//			allForecasts.erase(allForecasts.begin() + allForecasts.size() - nExtraForecasts, allForecasts.end());
////			cout<<"number of forecasts is bigger than the number of samples"<<endl;
////			cout<<"nExtraForecasts: "<< nExtraForecasts << endl;
////			exit(1);
//		}
//
//		return allForecasts;
//	}

//TODO -- Check if the above blindForecasting provides the same results as this one down here
//	vector<double> blindForecasting(const RepEFP& rep, const vector<vector<double> > vForecastings)
//	{
//
//		//cout<<"availableForecasting = "<<availableForecasting<<endl;
//
//		int sizeSP = rep.singleIndex.size();
//		int sizeMP = rep.averageIndex.size();
//		int sizeDP = rep.derivativeIndex.size();
//
//		int nForecastingValidations = vForecastings[0].size();
//
//		int i = problemParam.getMaxLag();
//		int maxLag = problemParam.getMaxLag();
//		vector<double> predicteds;
//
//		for (int pa = 0; pa < stepsAhead; pa++)			// passos a frente
//		{
//			//forecasting estimation
//			double estimation = 0;
//			double greaterAccepeted = 0;
//			double lowerAccepted = 0;
//
//			for (int nSP = 0; nSP < sizeSP; nSP++)
//			{
//				int file = rep.singleIndex[nSP].first;
//				int K = rep.singleIndex[nSP].second;
//
//				double value = 0;
//
//				if (file == 0)
//				{
//					if (pa >= K)
//						value = predicteds[pa - K];
//					else
//						value = vForecastings[file][i + pa - K];
//
//				}
//				else
//				{
//					if ((i + pa - K) > vForecastings[file].size())
//					{
//						//cout << nForecastings << endl;
//						//cout << pEFP.getForecastings(file, i + pa - K) << endl;
//						//cout << i + pa - K << "\t" << i << "\t" << pa << "\t" << K << endl;
//						//getchar();
//						value = 0;
//					}
//					else
//						value = vForecastings[file][i + pa - K];
//				}
//
//				double ruleGreater = rep.singleFuzzyRS[nSP][GREATER];
//				double greaterWeight = rep.singleFuzzyRS[nSP][GREATER_WEIGHT];
//				double ruleLower = rep.singleFuzzyRS[nSP][LOWER];
//				double lowerWeight = rep.singleFuzzyRS[nSP][LOWER_WEIGHT];
//				double ruleEpsilon = rep.singleFuzzyRS[nSP][EPSILON];
//				FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.singleFuzzyRS[nSP][PERTINENCEFUNC]);
//
//				defuzzification(ruleGreater, greaterWeight, ruleLower, lowerWeight, ruleEpsilon, repFuzzyPertinenceFunc, value, estimation, greaterAccepeted, lowerAccepted);
//
//			}
//
//			for (int nMP = 0; nMP < sizeMP; nMP++)
//			{
//				vector<pair<int, int> > meansK = rep.averageIndex[nMP];
//				//cout << meansK << endl;
//				//getchar();
//				double mean = 0;
//				for (int mK = 0; mK < meansK.size(); mK++)
//				{
//					int file = meansK[mK].first;
//					int K = meansK[mK].second;
//
//					//cout << "K = " << K << endl;
//					if (file == 0)
//					{
//						if (pa >= K)
//						{
//							mean += predicteds[pa - K];
//
//						}
//						else
//						{
//							mean += vForecastings[file][i + pa - K];
//
//						}
//					}
//					else
//					{
//						mean += vForecastings[file][i + pa - K];
//					}
//				}
//
//				mean = mean / meansK.size();
//				//cout << mean << endl;
//
//				double ruleGreater = rep.averageFuzzyRS[nMP][GREATER];
//				double greaterWeight = rep.averageFuzzyRS[nMP][GREATER_WEIGHT];
//				double ruleLower = rep.averageFuzzyRS[nMP][LOWER];
//				double lowerWeight = rep.averageFuzzyRS[nMP][LOWER_WEIGHT];
//				double ruleEpsilon = rep.averageFuzzyRS[nMP][EPSILON];
//				FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.averageFuzzyRS[nMP][PERTINENCEFUNC]);
//
//				defuzzification(ruleGreater, greaterWeight, ruleLower, lowerWeight, ruleEpsilon, repFuzzyPertinenceFunc, mean, estimation, greaterAccepeted, lowerAccepted);
//
//			}
//
//			for (int nDP = 0; nDP < sizeDP; nDP++)
//			{
//				vector<pair<int, int> > derivateK = rep.derivativeIndex[nDP];
//				//cout << derivateK << endl;
//				//getchar();
//
//				double d = 0;
//				for (int dK = 0; dK < derivateK.size(); dK++)
//				{
//					int file = derivateK[dK].first;
//					int K = derivateK[dK].second;
//					//cout << "K = " << K << endl;
//
//					if (file == 0)
//					{
//						if (dK == 0)
//						{
//							if (pa >= K)
//								d += predicteds[pa - K];
//							else
//								d += vForecastings[file][i + pa - K];
//						}
//						else
//						{
//							if (pa >= K)
//								d -= predicteds[pa - K];
//							else
//								d -= vForecastings[file][i + pa - K];
//						}
//					}
//					else
//					{
//						if (dK == 0)
//						{
//							d += vForecastings[file][i + pa - K];
//						}
//						else
//						{
//							d -= vForecastings[file][i + pa - K];
//						}
//					}
//				}
//
//				double ruleGreater = rep.derivativeFuzzyRS[nDP][GREATER];
//				double greaterWeight = rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT];
//				double ruleLower = rep.derivativeFuzzyRS[nDP][LOWER];
//				double lowerWeight = rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT];
//				double ruleEpsilon = rep.derivativeFuzzyRS[nDP][EPSILON];
//				FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.derivativeFuzzyRS[nDP][PERTINENCEFUNC]);
//
//				defuzzification(ruleGreater, greaterWeight, ruleLower, lowerWeight, ruleEpsilon, repFuzzyPertinenceFunc, d, estimation, greaterAccepeted, lowerAccepted);
//
//			}
//
//			double accepted = greaterAccepeted + lowerAccepted;
//			if (accepted > 0)
//				estimation /= accepted;
//
//			if (aprox != 0)
//			{
//				double alpha = rep.alpha;
//				vector<double> vAlpha = rep.vAlpha;
//				vector<double> vIndexAlphas = rep.vIndexAlphas;
//				vector<int> vIndex = rep.vIndex;
//
//				approximationsEnayatifar(aprox, alpha, vAlpha, vIndexAlphas, vIndex, estimation, i, pa, vForecastings, predicteds, maxLag);
//			}
//
//			predicteds.push_back(estimation);
//
//		}
//
//		cout << "Returning blind forecasts..." << endl;
//
//		return predicteds;
//	}

};

}

#endif /*HFM_EVALUATOR_HPP_*/

