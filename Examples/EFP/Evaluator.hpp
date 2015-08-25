#ifndef EFP_EVALUATOR_HPP_
#define EFP_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#define EPSILON_EFP 0.0001

namespace EFP
{

const int MAPE_INDEX = 0;
const int PINBALL_INDEX = 1;
const int MSE_INDEX = 2;
const int RMSE_INDEX = 3;
const int NMETRICS = 4;
const int PINBALL_ERROR_INDEX = 4;

float sigmoid(float x)
{
	float exp_value;
	float return_value;

	/*** Exponential calculation ***/
	exp_value = exp((double) -x);

	/*** Final sigmoid value ***/
	return_value = 1 / (1 + exp_value);

	return return_value;
}

static bool compara(double d1, double d2)
{
	return d1 < d2;
}

class EFPEvaluator: public Evaluator<RepEFP>
{
private:
	ProblemInstance& pEFP;

	int optionsP;
	int nForecastings;
	int stepsAhead;
	int targetFile;

	int evalFO;
	ProblemParameters& problemParam;
	int aprox;

public:

	EFPEvaluator(ProblemInstance& _pEFP, ProblemParameters& _problemParam, int _evalFO, int _aprox) : // If necessary, add more parameters
			pEFP(_pEFP), problemParam(_problemParam), evalFO(_evalFO), aprox(_aprox)
	{
		targetFile = 0; //this is the file to be learned

		stepsAhead = problemParam.getStepsAhead();

		nForecastings = pEFP.getForecatingsSizeFile(0); //all files have the same size

		//calculating the oldest step used as input of the model

	}

	virtual ~EFPEvaluator()
	{

	}

	vector<double> returnForecastingsFO(const RepEFP& rep, const vector<vector<double> >& vForecastings, bool validationMode)
	{

		//cout << vForecastings << endl;
		//Fo vector with different metrics calculations
		vector<double> foIndicator(NMETRICS + 1, 0);
		vector<pair<double, double> > vPinballFunctions(20, make_pair(0, 0));

		/*vector<vector<double> > foIndicatorStepsAhead(NMETRICS + 1);
		 for (int i = 0; i < foIndicatorStepsAhead.size(); i++)
		 foIndicatorStepsAhead[i].resize(stepsAhead);*/

		//cout<<"availableForecasting = "<<availableForecasting<<endl;
		int sizeSP = rep.singleIndex.size();
		int sizeMP = rep.averageIndex.size();
		int sizeDP = rep.derivativeIndex.size();

		int nForTargetFile = vForecastings[0].size();

		//vector<double> allForecasted;
		//vector<double> allForecastedTrue;
		int earliestInput = rep.earliestInput;

		/*
		 cout<<earliestInput<<endl;
		 cout << rep.singleIndex << endl;
		 cout << rep.singleFuzzyRS << endl;
		 cout << rep.averageIndex << endl;
		 cout << rep.averageFuzzyRS << endl;
		 cout << rep.derivativeIndex << endl;
		 cout << rep.derivativeFuzzyRS << endl;
		 //getchar();*/
		vector<double> allForecasts;
		vector<double> allTrue;
		for (int i = earliestInput; i < nForTargetFile; i += stepsAhead) // main loop that varries all the time series
		{
			vector<double> predicteds;

			for (int pa = 0; ((pa < stepsAhead) && (pa + i < nForTargetFile)); pa++) //auxiliar loop for steps ahead
			{
				vector<double> fuzzyWeights;
				//forecasting estimation
				double estimation = 0;
				double greaterAccepeted = 0;
				double lowerAccepted = 0;

				for (int nSP = 0; nSP < sizeSP; nSP++)
				{
					int file = rep.singleIndex[nSP].first;
					int K = rep.singleIndex[nSP].second;
					FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.singleFuzzyRS[nSP][PERTINENCEFUNC]);

					double value = 0;

					if (file == 0)
					{
						if (pa >= K)
							value = predicteds[pa - K];
						else
							value = vForecastings[file][i + pa - K];

					}
					else
					{
						if ((i + pa - K) > vForecastings[file].size())
						{
							//cout << nForecastings << endl;
							//cout << pEFP.getForecastings(file, i + pa - K) << endl;
							//cout << i + pa - K << "\t" << i << "\t" << pa << "\t" << K << endl;
							//getchar();
							value = 0;
						}
						else
							value = vForecastings[file][i + pa - K];
					}

					if (repFuzzyPertinenceFunc == Heavisde)
					{
						if (value > rep.singleFuzzyRS[nSP][GREATER])
						{
							estimation += rep.singleFuzzyRS[nSP][GREATER_WEIGHT];
							greaterAccepeted += 1;
						}

						if (value < rep.singleFuzzyRS[nSP][LOWER])
						{
							estimation += rep.singleFuzzyRS[nSP][LOWER_WEIGHT];
							lowerAccepted += 1;
						}

					}

					//Trapezoid Function
					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.singleFuzzyRS[nSP][EPSILON];
						double est = 0;
						double a = rep.singleFuzzyRS[nSP][GREATER];
						double mu = 0;
						if (value <= (a - epsilon))
							mu = 0;
						if (value > a)
							mu = 1;
						if ((value > (a - epsilon)) && value <= a)
						{
							double K1 = 1 / epsilon;
							double K2 = 1 - a * K1;
							double mu = value * K1 + K2;
						}

						est = rep.singleFuzzyRS[nSP][GREATER_WEIGHT] * mu;
						estimation += est;

						greaterAccepeted += mu;
					}

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.singleFuzzyRS[nSP][EPSILON];
						double b = rep.singleFuzzyRS[nSP][LOWER];
						double est = 0;
						double mu = 0;
						if (value >= (b + epsilon))
							mu = 0;
						if (value < b)
							mu = 1;
						if (value >= b && value < b + epsilon)
						{
							double K1 = 1 / epsilon;
							double K2 = 1 - b * K1;
							double mu = value * K1 + K2;
						}
						est = rep.singleFuzzyRS[nSP][LOWER_WEIGHT] * mu;
						estimation += est;

						lowerAccepted += mu;
					}

					fuzzyWeights.push_back(value);
				}

				for (int nMP = 0; nMP < sizeMP; nMP++)
				{
					vector<pair<int, int> > meansK = rep.averageIndex[nMP];
					//cout << meansK << endl;
					//getchar();
					double mean = 0;
					for (int mK = 0; mK < meansK.size(); mK++)
					{
						int file = meansK[mK].first;
						int K = meansK[mK].second;

						//cout << "K = " << K << endl;
						if (file == 0)
						{
							if (pa >= K)
							{
								mean += predicteds[pa - K];

							}
							else
							{
								mean += vForecastings[file][i + pa - K];

							}
						}
						else
						{
							mean += vForecastings[file][i + pa - K];
						}
					}

					mean = mean / meansK.size();
					//cout << mean << endl;

					FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.averageFuzzyRS[nMP][PERTINENCEFUNC]);

					if (repFuzzyPertinenceFunc == Heavisde)
					{
						if (mean > rep.averageFuzzyRS[nMP][GREATER])
						{
							estimation += rep.averageFuzzyRS[nMP][GREATER_WEIGHT];
							greaterAccepeted += 1;
						}

						if (mean < rep.averageFuzzyRS[nMP][LOWER])
						{
							estimation += rep.averageFuzzyRS[nMP][LOWER_WEIGHT];
							lowerAccepted += 1;
						}

					}

					/*
					 //Sigmoide Function
					 if (repFuzzyPertinenceFunc == Sigmoide)
					 {
					 double biggerDiff = mean - rep.averageFuzzyRS[nMP][GREATER];
					 double est = 0;
					 double mu = sigmoid(biggerDiff);
					 est = rep.averageFuzzyRS[nMP][GREATER_WEIGHT] * mu;
					 estimation += est;

					 greaterAccepeted += mu;
					 }

					 if (repFuzzyPertinenceFunc == Sigmoide)
					 {
					 double smallDiff = -mean + rep.averageFuzzyRS[nMP][LOWER];
					 double est = 0;
					 double mu = sigmoid(smallDiff);
					 est = rep.averageFuzzyRS[nMP][LOWER_WEIGHT] * mu;
					 estimation += est;

					 lowerAccepted += mu;
					 }*/

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.averageFuzzyRS[nMP][EPSILON];
						double est = 0;
						double a = rep.averageFuzzyRS[nMP][GREATER];
						double mu = 0;
						if (mean <= (a - epsilon))
							mu = 0;
						if (mean > a)
							mu = 1;
						if ((mean > (a - epsilon)) && mean <= a)
						{

							double K1 = 1 / epsilon;
							double K2 = 1 - a * K1;
							double mu = mean * K1 + K2;
							/*
							 cout << rep.singleFuzzyRS[nSP] << endl;
							 cout << value << "\t" << a << "\t" << epsilon << "\t" << mu << endl;
							 if (epsilon != 1)
							 getchar();*/
						}

						est = rep.averageFuzzyRS[nMP][GREATER_WEIGHT] * mu;
						estimation += est;

						greaterAccepeted += mu;
					}

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.averageFuzzyRS[nMP][EPSILON];
						double b = rep.averageFuzzyRS[nMP][LOWER];
						double est = 0;
						double mu = 0;
						if (mean >= (b + epsilon))
							mu = 0;
						if (mean < b)
							mu = 1;
						if (mean >= b && mean < b + epsilon)
						{
							double K1 = 1 / epsilon;
							double K2 = 1 - b * K1;
							double mu = mean * K1 + K2;
						}
						est = rep.averageFuzzyRS[nMP][LOWER_WEIGHT] * mu;
						estimation += est;

						lowerAccepted += mu;
					}

					fuzzyWeights.push_back(mean);
				}

				for (int nDP = 0; nDP < sizeDP; nDP++)
				{
					vector<pair<int, int> > derivateK = rep.derivativeIndex[nDP];
					//cout << derivateK << endl;
					//getchar();

					double d = 0;
					for (int dK = 0; dK < derivateK.size(); dK++)
					{
						int file = derivateK[dK].first;
						int K = derivateK[dK].second;
						//cout << "K = " << K << endl;

						if (file == 0)
						{
							if (dK == 0)
							{
								if (pa >= K)
									d += predicteds[pa - K];
								else
									d += vForecastings[file][i + pa - K];
							}
							else
							{
								if (pa >= K)
									d -= predicteds[pa - K];
								else
									d -= vForecastings[file][i + pa - K];
							}
						}
						else
						{
							if (dK == 0)
							{
								d += vForecastings[file][i + pa - K];
							}
							else
							{
								d -= vForecastings[file][i + pa - K];
							}
						}
					}

					FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.derivativeFuzzyRS[nDP][PERTINENCEFUNC]);

					if (repFuzzyPertinenceFunc == Heavisde)
					{
						if (d > rep.derivativeFuzzyRS[nDP][GREATER])
						{
							estimation += rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT];
							greaterAccepeted += 1;
						}

						if (d < rep.derivativeFuzzyRS[nDP][LOWER])
						{
							estimation += rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT];
							lowerAccepted += 1;
						}

					}

					/*
					 //Sigmoide Function
					 if (repFuzzyPertinenceFunc == Sigmoide)
					 {
					 double biggerDiff = d - rep.derivativeFuzzyRS[nDP][GREATER];
					 double est = 0;
					 double mu = sigmoid(biggerDiff);
					 est = rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT] * mu;
					 estimation += est;

					 greaterAccepeted += mu;
					 }

					 if (repFuzzyPertinenceFunc == Sigmoide)
					 {
					 double smallDiff = -d + rep.derivativeFuzzyRS[nDP][LOWER];
					 double est = 0;
					 double mu = sigmoid(smallDiff);
					 est = rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT] * mu;
					 estimation += est;

					 lowerAccepted += mu;
					 }*/

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.derivativeFuzzyRS[nDP][EPSILON];
						double est = 0;
						double a = rep.derivativeFuzzyRS[nDP][GREATER];
						double mu = 0;
						if (d <= (a - epsilon))
							mu = 0;
						if (d > a)
							mu = 1;
						if ((d > (a - epsilon)) && d <= a)
						{

							double K1 = 1 / epsilon;
							double K2 = 1 - a * K1;
							double mu = d * K1 + K2;
							/*
							 cout << rep.singleFuzzyRS[nSP] << endl;
							 cout << value << "\t" << a << "\t" << epsilon << "\t" << mu << endl;
							 if (epsilon != 1)
							 getchar();*/
						}

						est = rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT] * mu;
						estimation += est;

						greaterAccepeted += mu;
					}

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.derivativeFuzzyRS[nDP][EPSILON];
						double b = rep.derivativeFuzzyRS[nDP][LOWER];
						double est = 0;
						double mu = 0;
						if (d >= (b + epsilon))
							mu = 0;
						if (d < b)
							mu = 1;
						if (d >= b && d < b + epsilon)
						{
							double K1 = 1 / epsilon;
							double K2 = 1 - b * K1;
							double mu = d * K1 + K2;
						}
						est = rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT] * mu;
						estimation += est;

						lowerAccepted += mu;
					}

					//cout << d << endl;
					fuzzyWeights.push_back(d);
				}

				//cout << "EstimationAntes:" << estimation << endl;
				double accepted = greaterAccepeted + lowerAccepted;
				if (accepted > 0)
					estimation /= accepted;

				if (aprox == 1)
				{
					double ajusts = 0;
					double alpha = rep.alpha;
					vector<int> adjustPrevValue;
					adjustPrevValue.push_back(1);
					adjustPrevValue.push_back(2);
					adjustPrevValue.push_back(24);
					adjustPrevValue.push_back(25);
					adjustPrevValue.push_back(167);
					adjustPrevValue.push_back(168);
					adjustPrevValue.push_back(169);

					for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
					vector<double> vAlpha = rep.vAlpha;
					vector<int> adjustPrevValue;
					adjustPrevValue.push_back(1);
					adjustPrevValue.push_back(2);
					adjustPrevValue.push_back(3);
					adjustPrevValue.push_back(23);
					adjustPrevValue.push_back(24);
					adjustPrevValue.push_back(25);
					for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
					vector<double> vIndexAlphas = rep.vIndexAlphas;
					vector<int> vIndex = rep.vIndex;

					for (int ind = 0; ind < vIndex.size(); ind++)
					{
						int indexK = vIndex[ind];
						double valueIndexK = 0;
						if (indexK <= i)
						{
							if (pa >= indexK)
								valueIndexK += predicteds[pa - indexK];
							else
								valueIndexK += vForecastings[targetFile][i + pa - indexK];

							ajusts += vIndexAlphas[ind] * (estimation - valueIndexK);
							if ((vIndex[ind] <= 0) || (vIndex[ind] > earliestInput))
							{

								cout << "Erro on INDEX of the adjustment";
								getchar();
							}
						}

					}

					estimation = estimation + ajusts;
				}

				if (aprox == 4)
				{
					double ajusts = 0;
					vector<double> vAlpha = rep.vAlpha;
					vector<int> adjustPrevValue;
					adjustPrevValue.push_back(1);
					adjustPrevValue.push_back(25);
					adjustPrevValue.push_back(26);
					adjustPrevValue.push_back(48);
					adjustPrevValue.push_back(168);

					for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
					vector<double> vAlpha = rep.vAlpha;
					vector<int> adjustPrevValue;
					adjustPrevValue.push_back(1);
					adjustPrevValue.push_back(24);
					adjustPrevValue.push_back(48);
					adjustPrevValue.push_back(168);
					adjustPrevValue.push_back(168 * 2);
					adjustPrevValue.push_back(168 * 3);
					for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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

				//TODO remove this for other forecast problem -- rain forecast
				if (estimation < 0)
					estimation = 0;

				//cout << rep << endl;
				//cout << "Estimation:" << estimation << endl;

				double forecastingTarget = vForecastings[targetFile][i + pa];
				//cout << "forecastingTarget:" << forecastingTarget << endl;
				if (forecastingTarget <= 0.001)
					forecastingTarget = 0.001;
				predicteds.push_back(estimation);
				allForecasts.push_back(estimation);
				allTrue.push_back(forecastingTarget);
				//allForecasted.push_back(estimation);
				//allForecastedTrue.push_back(forecastingTarget);

				//MAPE CALCULUSS
				if (evalFO == MAPE_INDEX || validationMode == true)
				{

					foIndicator[MAPE_INDEX] += (abs(estimation - forecastingTarget) / abs(forecastingTarget));
					//teste MAPE invertido
//					if (validationMode)
//					{
//						foIndicator[MAPE_INDEX] += (abs(estimation - forecastingTarget) / abs(forecastingTarget));
//					}
//					else
//					{
//						//foIndicator[MAPE_INDEX] += (abs(estimation - forecastingTarget) / abs(183 - forecastingTarget));
//						double a = 0;
//						a += (abs(estimation - forecastingTarget) / abs(forecastingTarget));
//						//a += (abs(estimation - forecastingTarget) / abs(183 - forecastingTarget));
//						foIndicator[MAPE_INDEX] += a;
//					}

					//maxRainLE 183
					//cout<<estimation<<endl;
					//cout<<forecastingTarget<<endl;
					//getchar();
				}

				// ===================  MAPE FOR EACH STEP AHEAD ========================
				//foIndicatorStepsAhead[MAPE_INDEX][pa] += (abs(estimation - forecastingTarget) / abs(forecastingTarget));
				// =================== FINISH MAPE FOR EACH STEP AHEAD ========================

				//PinballFunction
				if (evalFO == PINBALL_INDEX || validationMode == true)
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
					foIndicator[PINBALL_INDEX] += pinballFunctionQuantils;
				}
				/*
				 int vQ = 0;
				 for (double quantilError = 0; quantilError <= 1; quantilError = quantilError + 0.05)
				 {
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
				 vPinballFunctions[vQ].first += pinballFunctionQuantils;
				 vPinballFunctions[vQ].second = quantilError;
				 vQ++;
				 }*/

				//MSE CALCULUSS
				if (evalFO == MSE_INDEX || evalFO == RMSE_INDEX || validationMode == true)
					foIndicator[MSE_INDEX] += pow((estimation - forecastingTarget), 2);
			}

			//cout<<"foIndicator[0]: "<<foIndicator[0]<<endl;
			//getchar();
		}

		if (validationMode == true)
		{
//			cout<<allTrue<<endl;
//			cout<<allForecasts<<endl;
//			getchar();

			string saidaPrev = "./mGSaida";

			FILE* fPrev = fopen(saidaPrev.c_str(), "a");

			for (int i = 0; i < allForecasts.size(); i++)
			{
				fprintf(fPrev, "%.7f\t%.7f", allForecasts[i], allTrue[i]);
				fprintf(fPrev, "\n");
			}

			fclose(fPrev);

		}
		//MAPE FINAL CALC
		if (evalFO == MAPE_INDEX || validationMode == true)
		{
			foIndicator[MAPE_INDEX] *= 100;
			foIndicator[MAPE_INDEX] /= (nForTargetFile - earliestInput);
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
		if (evalFO == MSE_INDEX || validationMode == true)
			foIndicator[MSE_INDEX] /= (nForTargetFile - earliestInput);

		if (evalFO == RMSE_INDEX || validationMode == true)
			foIndicator[RMSE_INDEX] = sqrt(foIndicator[MSE_INDEX]);

		//PINBALL FINAL CALC
		if (evalFO == PINBALL_INDEX || validationMode == true)
		{
			foIndicator[PINBALL_INDEX] /= (nForTargetFile - earliestInput);
		}
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
		//getchar();
	}

	vector<double> returnForecastingsFOForecasts(const RepEFP& rep, const vector<vector<double> >& vForecastings, bool validationMode)
	{

		//cout << vForecastings << endl;
		//Fo vector with different metrics calculations
		vector<double> foIndicator(NMETRICS + 1, 0);
		vector<pair<double, double> > vPinballFunctions(20, make_pair(0, 0));

		/*vector<vector<double> > foIndicatorStepsAhead(NMETRICS + 1);
		 for (int i = 0; i < foIndicatorStepsAhead.size(); i++)
		 foIndicatorStepsAhead[i].resize(stepsAhead);*/

		//cout<<"availableForecasting = "<<availableForecasting<<endl;
		int sizeSP = rep.singleIndex.size();
		int sizeMP = rep.averageIndex.size();
		int sizeDP = rep.derivativeIndex.size();

		int nForTargetFile = vForecastings[0].size();

		//vector<double> allForecasted;
		//vector<double> allForecastedTrue;
		int earliestInput = rep.earliestInput;

		/*
		 cout<<earliestInput<<endl;
		 cout << rep.singleIndex << endl;
		 cout << rep.singleFuzzyRS << endl;
		 cout << rep.averageIndex << endl;
		 cout << rep.averageFuzzyRS << endl;
		 cout << rep.derivativeIndex << endl;
		 cout << rep.derivativeFuzzyRS << endl;
		 //getchar();*/
		vector<double> allForecasts;
		vector<double> allTrue;
		for (int i = earliestInput; i < nForTargetFile; i += stepsAhead) // main loop that varries all the time series
		{
			vector<double> predicteds;

			for (int pa = 0; ((pa < stepsAhead) && (pa + i < nForTargetFile)); pa++) //auxiliar loop for steps ahead
			{
				vector<double> fuzzyWeights;
				//forecasting estimation
				double estimation = 0;
				double greaterAccepeted = 0;
				double lowerAccepted = 0;

				for (int nSP = 0; nSP < sizeSP; nSP++)
				{
					int file = rep.singleIndex[nSP].first;
					int K = rep.singleIndex[nSP].second;
					FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.singleFuzzyRS[nSP][PERTINENCEFUNC]);

					double value = 0;

					if (file == 0)
					{
						if (pa >= K)
							value = predicteds[pa - K];
						else
							value = vForecastings[file][i + pa - K];

					}
					else
					{
						if ((i + pa - K) > vForecastings[file].size())
						{
							//cout << nForecastings << endl;
							//cout << pEFP.getForecastings(file, i + pa - K) << endl;
							//cout << i + pa - K << "\t" << i << "\t" << pa << "\t" << K << endl;
							//getchar();
							value = 0;
						}
						else
							value = vForecastings[file][i + pa - K];
					}

					if (repFuzzyPertinenceFunc == Heavisde)
					{
						if (value > rep.singleFuzzyRS[nSP][GREATER])
						{
							estimation += rep.singleFuzzyRS[nSP][GREATER_WEIGHT];
							greaterAccepeted += 1;
						}

						if (value < rep.singleFuzzyRS[nSP][LOWER])
						{
							estimation += rep.singleFuzzyRS[nSP][LOWER_WEIGHT];
							lowerAccepted += 1;
						}

					}

					//Trapezoid Function
					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.singleFuzzyRS[nSP][EPSILON];
						double est = 0;
						double a = rep.singleFuzzyRS[nSP][GREATER];
						double mu = 0;
						if (value <= (a - epsilon))
							mu = 0;
						if (value > a)
							mu = 1;
						if ((value > (a - epsilon)) && value <= a)
						{
							double K1 = 1 / epsilon;
							double K2 = 1 - a * K1;
							double mu = value * K1 + K2;
						}

						est = rep.singleFuzzyRS[nSP][GREATER_WEIGHT] * mu;
						estimation += est;

						greaterAccepeted += mu;
					}

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.singleFuzzyRS[nSP][EPSILON];
						double b = rep.singleFuzzyRS[nSP][LOWER];
						double est = 0;
						double mu = 0;
						if (value >= (b + epsilon))
							mu = 0;
						if (value < b)
							mu = 1;
						if (value >= b && value < b + epsilon)
						{
							double K1 = 1 / epsilon;
							double K2 = 1 - b * K1;
							double mu = value * K1 + K2;
						}
						est = rep.singleFuzzyRS[nSP][LOWER_WEIGHT] * mu;
						estimation += est;

						lowerAccepted += mu;
					}

					fuzzyWeights.push_back(value);
				}

				for (int nMP = 0; nMP < sizeMP; nMP++)
				{
					vector<pair<int, int> > meansK = rep.averageIndex[nMP];
					//cout << meansK << endl;
					//getchar();
					double mean = 0;
					for (int mK = 0; mK < meansK.size(); mK++)
					{
						int file = meansK[mK].first;
						int K = meansK[mK].second;

						//cout << "K = " << K << endl;
						if (file == 0)
						{
							if (pa >= K)
							{
								mean += predicteds[pa - K];

							}
							else
							{
								mean += vForecastings[file][i + pa - K];

							}
						}
						else
						{
							mean += vForecastings[file][i + pa - K];
						}
					}

					mean = mean / meansK.size();
					//cout << mean << endl;

					FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.averageFuzzyRS[nMP][PERTINENCEFUNC]);

					if (repFuzzyPertinenceFunc == Heavisde)
					{
						if (mean > rep.averageFuzzyRS[nMP][GREATER])
						{
							estimation += rep.averageFuzzyRS[nMP][GREATER_WEIGHT];
							greaterAccepeted += 1;
						}

						if (mean < rep.averageFuzzyRS[nMP][LOWER])
						{
							estimation += rep.averageFuzzyRS[nMP][LOWER_WEIGHT];
							lowerAccepted += 1;
						}

					}

					/*
					 //Sigmoide Function
					 if (repFuzzyPertinenceFunc == Sigmoide)
					 {
					 double biggerDiff = mean - rep.averageFuzzyRS[nMP][GREATER];
					 double est = 0;
					 double mu = sigmoid(biggerDiff);
					 est = rep.averageFuzzyRS[nMP][GREATER_WEIGHT] * mu;
					 estimation += est;

					 greaterAccepeted += mu;
					 }

					 if (repFuzzyPertinenceFunc == Sigmoide)
					 {
					 double smallDiff = -mean + rep.averageFuzzyRS[nMP][LOWER];
					 double est = 0;
					 double mu = sigmoid(smallDiff);
					 est = rep.averageFuzzyRS[nMP][LOWER_WEIGHT] * mu;
					 estimation += est;

					 lowerAccepted += mu;
					 }*/

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.averageFuzzyRS[nMP][EPSILON];
						double est = 0;
						double a = rep.averageFuzzyRS[nMP][GREATER];
						double mu = 0;
						if (mean <= (a - epsilon))
							mu = 0;
						if (mean > a)
							mu = 1;
						if ((mean > (a - epsilon)) && mean <= a)
						{

							double K1 = 1 / epsilon;
							double K2 = 1 - a * K1;
							double mu = mean * K1 + K2;
							/*
							 cout << rep.singleFuzzyRS[nSP] << endl;
							 cout << value << "\t" << a << "\t" << epsilon << "\t" << mu << endl;
							 if (epsilon != 1)
							 getchar();*/
						}

						est = rep.averageFuzzyRS[nMP][GREATER_WEIGHT] * mu;
						estimation += est;

						greaterAccepeted += mu;
					}

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.averageFuzzyRS[nMP][EPSILON];
						double b = rep.averageFuzzyRS[nMP][LOWER];
						double est = 0;
						double mu = 0;
						if (mean >= (b + epsilon))
							mu = 0;
						if (mean < b)
							mu = 1;
						if (mean >= b && mean < b + epsilon)
						{
							double K1 = 1 / epsilon;
							double K2 = 1 - b * K1;
							double mu = mean * K1 + K2;
						}
						est = rep.averageFuzzyRS[nMP][LOWER_WEIGHT] * mu;
						estimation += est;

						lowerAccepted += mu;
					}

					fuzzyWeights.push_back(mean);
				}

				for (int nDP = 0; nDP < sizeDP; nDP++)
				{
					vector<pair<int, int> > derivateK = rep.derivativeIndex[nDP];
					//cout << derivateK << endl;
					//getchar();

					double d = 0;
					for (int dK = 0; dK < derivateK.size(); dK++)
					{
						int file = derivateK[dK].first;
						int K = derivateK[dK].second;
						//cout << "K = " << K << endl;

						if (file == 0)
						{
							if (dK == 0)
							{
								if (pa >= K)
									d += predicteds[pa - K];
								else
									d += vForecastings[file][i + pa - K];
							}
							else
							{
								if (pa >= K)
									d -= predicteds[pa - K];
								else
									d -= vForecastings[file][i + pa - K];
							}
						}
						else
						{
							if (dK == 0)
							{
								d += vForecastings[file][i + pa - K];
							}
							else
							{
								d -= vForecastings[file][i + pa - K];
							}
						}
					}

					FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.derivativeFuzzyRS[nDP][PERTINENCEFUNC]);

					if (repFuzzyPertinenceFunc == Heavisde)
					{
						if (d > rep.derivativeFuzzyRS[nDP][GREATER])
						{
							estimation += rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT];
							greaterAccepeted += 1;
						}

						if (d < rep.derivativeFuzzyRS[nDP][LOWER])
						{
							estimation += rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT];
							lowerAccepted += 1;
						}

					}

					/*
					 //Sigmoide Function
					 if (repFuzzyPertinenceFunc == Sigmoide)
					 {
					 double biggerDiff = d - rep.derivativeFuzzyRS[nDP][GREATER];
					 double est = 0;
					 double mu = sigmoid(biggerDiff);
					 est = rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT] * mu;
					 estimation += est;

					 greaterAccepeted += mu;
					 }

					 if (repFuzzyPertinenceFunc == Sigmoide)
					 {
					 double smallDiff = -d + rep.derivativeFuzzyRS[nDP][LOWER];
					 double est = 0;
					 double mu = sigmoid(smallDiff);
					 est = rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT] * mu;
					 estimation += est;

					 lowerAccepted += mu;
					 }*/

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.derivativeFuzzyRS[nDP][EPSILON];
						double est = 0;
						double a = rep.derivativeFuzzyRS[nDP][GREATER];
						double mu = 0;
						if (d <= (a - epsilon))
							mu = 0;
						if (d > a)
							mu = 1;
						if ((d > (a - epsilon)) && d <= a)
						{

							double K1 = 1 / epsilon;
							double K2 = 1 - a * K1;
							double mu = d * K1 + K2;
							/*
							 cout << rep.singleFuzzyRS[nSP] << endl;
							 cout << value << "\t" << a << "\t" << epsilon << "\t" << mu << endl;
							 if (epsilon != 1)
							 getchar();*/
						}

						est = rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT] * mu;
						estimation += est;

						greaterAccepeted += mu;
					}

					if (repFuzzyPertinenceFunc == Trapezoid)
					{
						double epsilon = rep.derivativeFuzzyRS[nDP][EPSILON];
						double b = rep.derivativeFuzzyRS[nDP][LOWER];
						double est = 0;
						double mu = 0;
						if (d >= (b + epsilon))
							mu = 0;
						if (d < b)
							mu = 1;
						if (d >= b && d < b + epsilon)
						{
							double K1 = 1 / epsilon;
							double K2 = 1 - b * K1;
							double mu = d * K1 + K2;
						}
						est = rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT] * mu;
						estimation += est;

						lowerAccepted += mu;
					}

					//cout << d << endl;
					fuzzyWeights.push_back(d);
				}

				//cout << "EstimationAntes:" << estimation << endl;
				double accepted = greaterAccepeted + lowerAccepted;
				if (accepted > 0)
					estimation /= accepted;

				if (aprox == 1)
				{
					double ajusts = 0;
					double alpha = rep.alpha;
					vector<int> adjustPrevValue;
					adjustPrevValue.push_back(1);
					adjustPrevValue.push_back(2);
					adjustPrevValue.push_back(24);
					adjustPrevValue.push_back(25);
					adjustPrevValue.push_back(167);
					adjustPrevValue.push_back(168);
					adjustPrevValue.push_back(169);

					for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
					vector<double> vAlpha = rep.vAlpha;
					vector<int> adjustPrevValue;
					adjustPrevValue.push_back(1);
					adjustPrevValue.push_back(2);
					adjustPrevValue.push_back(3);
					adjustPrevValue.push_back(23);
					adjustPrevValue.push_back(24);
					adjustPrevValue.push_back(25);
					for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
					vector<double> vIndexAlphas = rep.vIndexAlphas;
					vector<int> vIndex = rep.vIndex;

					for (int ind = 0; ind < vIndex.size(); ind++)
					{
						int indexK = vIndex[ind];
						double valueIndexK = 0;
						if (indexK <= i)
						{
							if (pa >= indexK)
								valueIndexK += predicteds[pa - indexK];
							else
								valueIndexK += vForecastings[targetFile][i + pa - indexK];

							ajusts += vIndexAlphas[ind] * (estimation - valueIndexK);
							if ((vIndex[ind] <= 0) || (vIndex[ind] > earliestInput))
							{

								cout << "Erro on INDEX of the adjustment";
								getchar();
							}
						}

					}

					estimation = estimation + ajusts;
				}

				if (aprox == 4)
				{
					double ajusts = 0;
					vector<double> vAlpha = rep.vAlpha;
					vector<int> adjustPrevValue;
					adjustPrevValue.push_back(1);
					adjustPrevValue.push_back(25);
					adjustPrevValue.push_back(26);
					adjustPrevValue.push_back(48);
					adjustPrevValue.push_back(168);

					for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
					vector<double> vAlpha = rep.vAlpha;
					vector<int> adjustPrevValue;
					adjustPrevValue.push_back(1);
					adjustPrevValue.push_back(24);
					adjustPrevValue.push_back(48);
					adjustPrevValue.push_back(168);
					adjustPrevValue.push_back(168 * 2);
					adjustPrevValue.push_back(168 * 3);
					for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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

				//TODO remove this for other forecast problem -- rain forecast
				if (estimation < 0)
					estimation = 0;
				//cout << rep << endl;
				//cout << "Estimation:" << estimation << endl;

				double forecastingTarget = vForecastings[targetFile][i + pa];
				//cout << "forecastingTarget:" << forecastingTarget << endl;

				predicteds.push_back(estimation);
				allForecasts.push_back(estimation);
				allTrue.push_back(forecastingTarget);
				//allForecasted.push_back(estimation);
				//allForecastedTrue.push_back(forecastingTarget);

				//MAPE CALCULUSS
				if (evalFO == MAPE_INDEX || validationMode == true)
				{
					double forecastingTargetMAPE = forecastingTarget;
					if (forecastingTargetMAPE == 0)
						forecastingTargetMAPE = 0.001;

					foIndicator[MAPE_INDEX] += (abs(estimation - forecastingTargetMAPE) / abs(forecastingTargetMAPE));
				}
				// ===================  MAPE FOR EACH STEP AHEAD ========================
				//foIndicatorStepsAhead[MAPE_INDEX][pa] += (abs(estimation - forecastingTarget) / abs(forecastingTarget));
				// =================== FINISH MAPE FOR EACH STEP AHEAD ========================

				//PinballFunction
				if (evalFO == PINBALL_INDEX || validationMode == true)
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
					foIndicator[PINBALL_INDEX] += pinballFunctionQuantils;
				}
				/*
				 int vQ = 0;
				 for (double quantilError = 0; quantilError <= 1; quantilError = quantilError + 0.05)
				 {
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
				 vPinballFunctions[vQ].first += pinballFunctionQuantils;
				 vPinballFunctions[vQ].second = quantilError;
				 vQ++;
				 }*/

				//MSE CALCULUSS
				if (evalFO == MSE_INDEX || evalFO == RMSE_INDEX || validationMode == true)
					foIndicator[MSE_INDEX] += pow((estimation - forecastingTarget), 2);
			}

			//cout<<"foIndicator[0]: "<<foIndicator[0]<<endl;
			//getchar();
		}

		if (validationMode == true)
		{
			//cout<<allTrue;
			//cout<<allForecasts;
			//getchar();

			string saidaPrev = "./24SAHibrido";

			FILE* fPrev = fopen(saidaPrev.c_str(), "a");

			for (int i = 0; i < allForecasts.size(); i++)
			{
				fprintf(fPrev, "%.7f\t%.7f", allForecasts[i], allTrue[i]);
				fprintf(fPrev, "\n");
			}

			fclose(fPrev);

		}
		//MAPE FINAL CALC
		if (evalFO == MAPE_INDEX || validationMode == true)
		{
			foIndicator[MAPE_INDEX] *= 100;
			foIndicator[MAPE_INDEX] /= (nForTargetFile - earliestInput);
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
		if (evalFO == MSE_INDEX || validationMode == true)
			foIndicator[MSE_INDEX] /= (nForTargetFile - earliestInput);

		if (evalFO == RMSE_INDEX || validationMode == true)
			foIndicator[RMSE_INDEX] = sqrt(foIndicator[MSE_INDEX]);

		//PINBALL FINAL CALC
		if (evalFO == PINBALL_INDEX || validationMode == true)
		{
			foIndicator[PINBALL_INDEX] /= (nForTargetFile - earliestInput);
		}
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
		return allForecasts;
		//getchar();
	}

	vector<double> blindForecasting(const RepEFP& rep, const vector<vector<double> > vForecastings)
	{

		//cout<<"availableForecasting = "<<availableForecasting<<endl;

		int sizeSP = rep.singleIndex.size();
		int sizeMP = rep.averageIndex.size();
		int sizeDP = rep.derivativeIndex.size();

		int nForecastingValidations = vForecastings[0].size();

		int i = rep.earliestInput;
		int earliestInput = rep.earliestInput;
		vector<double> predicteds;

		for (int pa = 0; pa < stepsAhead; pa++)			// passos a frente
		{
			vector<double> fuzzyWeights;
			//forecasting estimation
			double estimation = 0;
			double greaterAccepeted = 0;
			double lowerAccepted = 0;

			for (int nSP = 0; nSP < sizeSP; nSP++)
			{
				int file = rep.singleIndex[nSP].first;
				int K = rep.singleIndex[nSP].second;
				FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.singleFuzzyRS[nSP][PERTINENCEFUNC]);

				double value = 0;

				if (file == 0)
				{
					if (pa >= K)
						value = predicteds[pa - K];
					else
						value = vForecastings[file][i + pa - K];

				}
				else
				{
					if ((i + pa - K) > vForecastings[file].size())
					{
						//cout << nForecastings << endl;
						//cout << pEFP.getForecastings(file, i + pa - K) << endl;
						//cout << i + pa - K << "\t" << i << "\t" << pa << "\t" << K << endl;
						//getchar();
						value = 0;
					}
					else
						value = vForecastings[file][i + pa - K];
				}

				if (repFuzzyPertinenceFunc == Heavisde)
				{
					if (value > rep.singleFuzzyRS[nSP][GREATER])
					{
						estimation += rep.singleFuzzyRS[nSP][GREATER_WEIGHT];
						greaterAccepeted += 1;
					}

					if (value < rep.singleFuzzyRS[nSP][LOWER])
					{
						estimation += rep.singleFuzzyRS[nSP][LOWER_WEIGHT];
						lowerAccepted += 1;
					}

				}

				//Trapezoid Function
				if (repFuzzyPertinenceFunc == Trapezoid)
				{
					double epsilon = rep.singleFuzzyRS[nSP][EPSILON];
					double est = 0;
					double a = rep.singleFuzzyRS[nSP][GREATER];
					double mu = 0;
					if (value <= (a - epsilon))
						mu = 0;
					if (value > a)
						mu = 1;
					if ((value > (a - epsilon)) && value <= a)
					{
						double K1 = 1 / epsilon;
						double K2 = 1 - a * K1;
						double mu = value * K1 + K2;
					}

					est = rep.singleFuzzyRS[nSP][GREATER_WEIGHT] * mu;
					estimation += est;

					greaterAccepeted += mu;
				}

				if (repFuzzyPertinenceFunc == Trapezoid)
				{
					double epsilon = rep.singleFuzzyRS[nSP][EPSILON];
					double b = rep.singleFuzzyRS[nSP][LOWER];
					double est = 0;
					double mu = 0;
					if (value >= (b + epsilon))
						mu = 0;
					if (value < b)
						mu = 1;
					if (value >= b && value < b + epsilon)
					{
						double K1 = 1 / epsilon;
						double K2 = 1 - b * K1;
						double mu = value * K1 + K2;
					}
					est = rep.singleFuzzyRS[nSP][LOWER_WEIGHT] * mu;
					estimation += est;

					lowerAccepted += mu;
				}

				fuzzyWeights.push_back(value);
			}

			for (int nMP = 0; nMP < sizeMP; nMP++)
			{
				vector<pair<int, int> > meansK = rep.averageIndex[nMP];
				//cout << meansK << endl;
				//getchar();
				double mean = 0;
				for (int mK = 0; mK < meansK.size(); mK++)
				{
					int file = meansK[mK].first;
					int K = meansK[mK].second;

					//cout << "K = " << K << endl;
					if (file == 0)
					{
						if (pa >= K)
						{
							mean += predicteds[pa - K];

						}
						else
						{
							mean += vForecastings[file][i + pa - K];

						}
					}
					else
					{
						mean += vForecastings[file][i + pa - K];
					}
				}

				mean = mean / meansK.size();
				//cout << mean << endl;

				FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.averageFuzzyRS[nMP][PERTINENCEFUNC]);

				if (repFuzzyPertinenceFunc == Heavisde)
				{
					if (mean > rep.averageFuzzyRS[nMP][GREATER])
					{
						estimation += rep.averageFuzzyRS[nMP][GREATER_WEIGHT];
						greaterAccepeted += 1;
					}

					if (mean < rep.averageFuzzyRS[nMP][LOWER])
					{
						estimation += rep.averageFuzzyRS[nMP][LOWER_WEIGHT];
						lowerAccepted += 1;
					}

				}

				/*
				 //Sigmoide Function
				 if (repFuzzyPertinenceFunc == Sigmoide)
				 {
				 double biggerDiff = mean - rep.averageFuzzyRS[nMP][GREATER];
				 double est = 0;
				 double mu = sigmoid(biggerDiff);
				 est = rep.averageFuzzyRS[nMP][GREATER_WEIGHT] * mu;
				 estimation += est;

				 greaterAccepeted += mu;
				 }

				 if (repFuzzyPertinenceFunc == Sigmoide)
				 {
				 double smallDiff = -mean + rep.averageFuzzyRS[nMP][LOWER];
				 double est = 0;
				 double mu = sigmoid(smallDiff);
				 est = rep.averageFuzzyRS[nMP][LOWER_WEIGHT] * mu;
				 estimation += est;

				 lowerAccepted += mu;
				 }*/

				if (repFuzzyPertinenceFunc == Trapezoid)
				{
					double epsilon = rep.averageFuzzyRS[nMP][EPSILON];
					double est = 0;
					double a = rep.averageFuzzyRS[nMP][GREATER];
					double mu = 0;
					if (mean <= (a - epsilon))
						mu = 0;
					if (mean > a)
						mu = 1;
					if ((mean > (a - epsilon)) && mean <= a)
					{

						double K1 = 1 / epsilon;
						double K2 = 1 - a * K1;
						double mu = mean * K1 + K2;
						/*
						 cout << rep.singleFuzzyRS[nSP] << endl;
						 cout << value << "\t" << a << "\t" << epsilon << "\t" << mu << endl;
						 if (epsilon != 1)
						 getchar();*/
					}

					est = rep.averageFuzzyRS[nMP][GREATER_WEIGHT] * mu;
					estimation += est;

					greaterAccepeted += mu;
				}

				if (repFuzzyPertinenceFunc == Trapezoid)
				{
					double epsilon = rep.averageFuzzyRS[nMP][EPSILON];
					double b = rep.averageFuzzyRS[nMP][LOWER];
					double est = 0;
					double mu = 0;
					if (mean >= (b + epsilon))
						mu = 0;
					if (mean < b)
						mu = 1;
					if (mean >= b && mean < b + epsilon)
					{
						double K1 = 1 / epsilon;
						double K2 = 1 - b * K1;
						double mu = mean * K1 + K2;
					}
					est = rep.averageFuzzyRS[nMP][LOWER_WEIGHT] * mu;
					estimation += est;

					lowerAccepted += mu;
				}

				fuzzyWeights.push_back(mean);
			}

			for (int nDP = 0; nDP < sizeDP; nDP++)
			{
				vector<pair<int, int> > derivateK = rep.derivativeIndex[nDP];
				//cout << derivateK << endl;
				//getchar();

				double d = 0;
				for (int dK = 0; dK < derivateK.size(); dK++)
				{
					int file = derivateK[dK].first;
					int K = derivateK[dK].second;
					//cout << "K = " << K << endl;

					if (file == 0)
					{
						if (dK == 0)
						{
							if (pa >= K)
								d += predicteds[pa - K];
							else
								d += vForecastings[file][i + pa - K];
						}
						else
						{
							if (pa >= K)
								d -= predicteds[pa - K];
							else
								d -= vForecastings[file][i + pa - K];
						}
					}
					else
					{
						if (dK == 0)
						{
							d += vForecastings[file][i + pa - K];
						}
						else
						{
							d -= vForecastings[file][i + pa - K];
						}
					}
				}

				FuzzyFunction repFuzzyPertinenceFunc = FuzzyFunction(rep.derivativeFuzzyRS[nDP][PERTINENCEFUNC]);

				if (repFuzzyPertinenceFunc == Heavisde)
				{
					if (d > rep.derivativeFuzzyRS[nDP][GREATER])
					{
						estimation += rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT];
						greaterAccepeted += 1;
					}

					if (d < rep.derivativeFuzzyRS[nDP][LOWER])
					{
						estimation += rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT];
						lowerAccepted += 1;
					}

				}

				/*
				 //Sigmoide Function
				 if (repFuzzyPertinenceFunc == Sigmoide)
				 {
				 double biggerDiff = d - rep.derivativeFuzzyRS[nDP][GREATER];
				 double est = 0;
				 double mu = sigmoid(biggerDiff);
				 est = rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT] * mu;
				 estimation += est;

				 greaterAccepeted += mu;
				 }

				 if (repFuzzyPertinenceFunc == Sigmoide)
				 {
				 double smallDiff = -d + rep.derivativeFuzzyRS[nDP][LOWER];
				 double est = 0;
				 double mu = sigmoid(smallDiff);
				 est = rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT] * mu;
				 estimation += est;

				 lowerAccepted += mu;
				 }*/

				if (repFuzzyPertinenceFunc == Trapezoid)
				{
					double epsilon = rep.derivativeFuzzyRS[nDP][EPSILON];
					double est = 0;
					double a = rep.derivativeFuzzyRS[nDP][GREATER];
					double mu = 0;
					if (d <= (a - epsilon))
						mu = 0;
					if (d > a)
						mu = 1;
					if ((d > (a - epsilon)) && d <= a)
					{

						double K1 = 1 / epsilon;
						double K2 = 1 - a * K1;
						double mu = d * K1 + K2;
						/*
						 cout << rep.singleFuzzyRS[nSP] << endl;
						 cout << value << "\t" << a << "\t" << epsilon << "\t" << mu << endl;
						 if (epsilon != 1)
						 getchar();*/
					}

					est = rep.derivativeFuzzyRS[nDP][GREATER_WEIGHT] * mu;
					estimation += est;

					greaterAccepeted += mu;
				}

				if (repFuzzyPertinenceFunc == Trapezoid)
				{
					double epsilon = rep.derivativeFuzzyRS[nDP][EPSILON];
					double b = rep.derivativeFuzzyRS[nDP][LOWER];
					double est = 0;
					double mu = 0;
					if (d >= (b + epsilon))
						mu = 0;
					if (d < b)
						mu = 1;
					if (d >= b && d < b + epsilon)
					{
						double K1 = 1 / epsilon;
						double K2 = 1 - b * K1;
						double mu = d * K1 + K2;
					}
					est = rep.derivativeFuzzyRS[nDP][LOWER_WEIGHT] * mu;
					estimation += est;

					lowerAccepted += mu;
				}

				//cout << d << endl;
				fuzzyWeights.push_back(d);
			}

			//cout << "EstimationAntes:" << estimation << endl;
			double accepted = greaterAccepeted + lowerAccepted;
			if (accepted > 0)
				estimation /= accepted;

			if (aprox == 1)
			{
				double ajusts = 0;
				double alpha = rep.alpha;
				vector<int> adjustPrevValue;
				adjustPrevValue.push_back(1);
				adjustPrevValue.push_back(2);
				adjustPrevValue.push_back(24);
				adjustPrevValue.push_back(25);
				adjustPrevValue.push_back(167);
				adjustPrevValue.push_back(168);
				adjustPrevValue.push_back(169);

				for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
				vector<double> vAlpha = rep.vAlpha;
				vector<int> adjustPrevValue;
				adjustPrevValue.push_back(1);
				adjustPrevValue.push_back(2);
				adjustPrevValue.push_back(3);
				adjustPrevValue.push_back(23);
				adjustPrevValue.push_back(24);
				adjustPrevValue.push_back(25);
				for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
				vector<double> vIndexAlphas = rep.vIndexAlphas;
				vector<int> vIndex = rep.vIndex;

				for (int ind = 0; ind < vIndex.size(); ind++)
				{
					int indexK = vIndex[ind];
					double valueIndexK = 0;
					if (indexK <= i)
					{
						if (pa >= indexK)
							valueIndexK += predicteds[pa - indexK];
						else
							valueIndexK += vForecastings[targetFile][i + pa - indexK];

						ajusts += vIndexAlphas[ind] * (estimation - valueIndexK);
						if ((vIndex[ind] <= 0) || (vIndex[ind] > earliestInput))
						{

							cout << "Erro on INDEX of the adjustment";
							getchar();
						}
					}

				}

				estimation = estimation + ajusts;
			}

			if (aprox == 4)
			{
				double ajusts = 0;
				vector<double> vAlpha = rep.vAlpha;
				vector<int> adjustPrevValue;
				adjustPrevValue.push_back(1);
				adjustPrevValue.push_back(25);
				adjustPrevValue.push_back(26);
				adjustPrevValue.push_back(48);
				adjustPrevValue.push_back(168);

				for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
				vector<double> vAlpha = rep.vAlpha;
				vector<int> adjustPrevValue;
				adjustPrevValue.push_back(1);
				adjustPrevValue.push_back(24);
				adjustPrevValue.push_back(48);
				adjustPrevValue.push_back(168);
				adjustPrevValue.push_back(168 * 2);
				adjustPrevValue.push_back(168 * 3);
				for (int ind = 0; ind < adjustPrevValue.size(); ind++)
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
			//cout << rep << endl;
			//cout << "Estimation:" << estimation << endl;

			predicteds.push_back(estimation);

			//MAPE CALCULUSS

			//cout<<"foIndicator[0]: "<<foIndicator[0]<<endl;
			//getchar();
		}

		cout << "Blind Forecasting performed and file generated succesfully!" << endl;

		return predicteds;
	}

	EvaluationEFP& evaluate(const RepEFP& rep)
	{
		//Fo vector with different metrics calculations
		vector<double> foIndicator(NMETRICS + 1, 0); //nMetrics + ErrorPinball

		/*
		 vector<vector<double> > foIndicatorStepsAhead(NMETRICS + 1);
		 for (int i = 0; i < foIndicatorStepsAhead.size(); i++)
		 foIndicatorStepsAhead[i].resize(stepsAhead);*/

		vector<vector<double> > vForecastings = pEFP.getForecastingsVector();

		foIndicator = returnForecastingsFO(rep, vForecastings, false); //validation mode == false

		double fo = foIndicator[evalFO];		// Evaluation Function Value

		return *new EvaluationEFP(fo);
	}

	virtual bool betterThan(double a, double b)
	{
		// MINIMIZATION
		return (a < (b - EPSILON_EFP));
	}

	virtual bool isMinimization() const
	{
		return true;
	}

	static string idComponent()
	{
		return "OptFrame:Evaluator:EFPEvaluator";
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}

#endif /*EFP_EVALUATOR_HPP_*/

