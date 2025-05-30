// ===================================
// Main.cpp file generated by OptFrame
// Project EFP
// ===================================

#include <math.h>
#include <stdlib.h>

#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>
#include <iomanip>
#include <iostream>
#include <numeric>

#include "../ForecastClass.hpp"

using namespace std;
using namespace optframe;
using namespace HFM;

int hosseinBlindForecasts(int argc, char** argv) {
  std::cout << "Welcome to Hossein evaluation model " << std::endl;
  RandGenMersenneTwister rg;
  sref<RandGen> rg2{new RandGenMersenneTwister{}};
  // long  1412730737
  long seed = time(nullptr);  // CalibrationMode
  // seed = 9;
  std::cout << "Seed = " << seed << std::endl;
  srand(seed);
  rg.setSeed(seed);

  int argvMaxLagRate = 30;
  int argvTimeES = 10;
  int argvFH = 24;
  //===================================
  std::cout << "Parametros:" << std::endl;
  std::cout << "argvMaxLagRate=" << argvMaxLagRate << std::endl;
  std::cout << "argvTimeES=" << argvTimeES << std::endl;
  std::cout << "argvFH=" << argvFH << std::endl;

  //	File* fileWCCIInstances;
  string hosseinInstance =
      "./MyProjects/HFM/Instance/hosseinEnergy/hosseinLoadData";

  vector<string> hosseinBlindLoad;
  hosseinBlindLoad.push_back(hosseinInstance.c_str());

  treatForecasts rF(hosseinBlindLoad);

  vector<vector<double>>
      vfoIndicatorCalibration;  // vector with the FO of each batch

  vector<SolutionEFP>
      vSolutionsBatches;  // vector with the solution of each batch

  for (int n = 0; n < 1; n++) {
    int mu = 100;
    int lambda = mu * 6;
    int evalFOMinimizer = MAPE_INDEX;
    int contructiveNumberOfRules = 100;
    int evalAprox = 0;
    double alphaACF = 0;
    int construtive = 2;
    // ============ END FORCES ======================

    // ============= METHOD PARAMETERS=================
    HFMParams methodParam;
    // seting up Continous ES params
    methodParam.setESInitialDesv(10);
    methodParam.setESMutationDesv(20);
    methodParam.setESMaxG(100000);

    // seting up ES params
    methodParam.setESMU(mu);
    methodParam.setESLambda(lambda);

    // seting up ACF construtive params
    methodParam.setConstrutiveMethod(construtive);
    methodParam.setConstrutivePrecision(contructiveNumberOfRules);
    vector<double> vAlphaACFlimits;
    vAlphaACFlimits.push_back(alphaACF);
    methodParam.setConstrutiveLimitAlphaACF(vAlphaACFlimits);

    // seting up Eval params
    methodParam.setEvalAprox(evalAprox);
    methodParam.setEvalFOMinimizer(evalFOMinimizer);
    // ==========================================

    // ================== READ FILE ============== CONSTRUTIVE 0 AND 1
    ProblemParameters problemParam;
    // ProblemParameters problemParam(vParametersFiles[randomParametersFiles]);
    int nSA = argvFH;
    problemParam.setStepsAhead(nSA);
    int stepsAhead = problemParam.getStepsAhead();

    int nTotalForecastingsTrainningSet = rF.getForecastsSize(0);

    //========SET PROBLEM MAXIMUM LAG ===============
    std::cout << "argvMaxLagRate = " << argvMaxLagRate << std::endl;

    int iterationMaxLag =
        ((nTotalForecastingsTrainningSet - stepsAhead) * argvMaxLagRate) /
        100.0;
    iterationMaxLag = ceil(iterationMaxLag);
    if (iterationMaxLag > (nTotalForecastingsTrainningSet - stepsAhead))
      iterationMaxLag--;
    if (iterationMaxLag <= 0) iterationMaxLag = 1;

    problemParam.setMaxLag(iterationMaxLag);
    int maxLag = problemParam.getMaxLag(0);

    // If maxUpperLag is greater than 0 model uses predicted data
    problemParam.setMaxUpperLag(0);
    // int maxUpperLag = problemParam.getMaxUpperLag();
    //=================================================

    int timeES = argvTimeES;  // online training time

    vector<double> foIndicators;

    int beginTrainingSet = 0;
    // int nTrainningRounds = 3;
    // int nTotalForecastingsTrainningSet = maxLag + nTrainningRounds *
    // stepsAhead;

    std::cout << std::setprecision(9);
    std::cout << std::fixed;
    double NTRaprox =
        (nTotalForecastingsTrainningSet - maxLag) / double(stepsAhead);
    std::cout << "BeginTrainninningSet: " << beginTrainingSet << std::endl;
    std::cout << "#nTotalForecastingsTrainningSet: "
              << nTotalForecastingsTrainningSet << std::endl;
    std::cout << "#~NTR: " << NTRaprox << std::endl;
    std::cout << "#sizeTrainingSet: " << rF.getForecastsSize(0) << std::endl;
    std::cout << "#maxNotUsed: " << maxLag << std::endl;
    std::cout << "#StepsAhead: " << stepsAhead << std::endl << std::endl;

    vector<vector<double>> trainningSet;  // trainningSetVector
    trainningSet.push_back(
        rF.getPartsForecastsEndToBegin(0, 0, nTotalForecastingsTrainningSet));

    ForecastClass forecastObject{trainningSet, problemParam, rg2, methodParam};

    std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;
    sol = forecastObject.run(timeES, 0, 0);
    //		cout<<sol->first.getR()<<endl;
    //		getchar();

    vector<vector<double>> dataForBlindForecasts;
    dataForBlindForecasts.push_back(
        rF.getPartsForecastsEndToBegin(0, 0, maxLag));
    vector<double>* blindForecasts =
        forecastObject.returnBlind(sol->first.getR(), dataForBlindForecasts);

    std::cout << blindForecasts << std::endl;

    string resultsWCCIComp = "./HosseinBlindResults";

    FILE* fResults = fopen(resultsWCCIComp.c_str(), "a");
    fprintf(fResults, "%d\t%d\n", argvTimeES, argvMaxLagRate);
    for (int n = 0; n < stepsAhead; n++) {
      fprintf(fResults, "%f;", blindForecasts->at(n));
    }
    fprintf(fResults, "\n");
  }

  return 0;
}
