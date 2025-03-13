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

using namespace std;
using namespace optframe;
using namespace HFM;

int mokokoWindSotavento(int argc, char** argv) {
  cout << "Welcome to Mokoko Split Wind Sotavento  calibration!" << endl;
  RandGenMersenneTwister rg;
  // long  1412730737
  long seed = time(nullptr);  // CalibrationMode
  // seed = 9;
  cout << "Seed = " << seed << endl;
  srand(seed);
  rg.setSeed(seed);

  int argvMaxLagRate = 40;
  int argvTimeES = 10;
  int argvFH = 24;
  int argvNEXP = 3;

  cout << "=========================================" << endl;
  cout << std::setprecision(3);
  cout << std::fixed;
  cout << "Parametros:" << endl;
  cout << "argvMaxLagRate=" << argvMaxLagRate << endl;
  cout << "argvTimeES=" << argvTimeES << endl;
  cout << "argvFH=" << argvFH << endl;
  cout << "argvNEXP=" << argvNEXP << endl;
  cout << "=========================================" << endl << endl;

  vector<vector<double>>
      vfoIndicatorCalibration;  // vector with the FO of each batch

  vector<SolutionEFP>
      vSolutionsBatches;  // vector with the solution of each batch
  int nBatches = 1;
  for (int n = 0; n < nBatches; n++) {
    int mu = 100;
    int lambda = mu * 6;
    int evalFOMinimizer = RMSE_INDEX;
    int contructiveNumberOfRules = 10;
    int evalAprox = 0;
    double alphaACF = 0.8;
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
    for (int nEXP = 1; nEXP < argvNEXP; nEXP++)
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

    vector<string> completeTS;
    completeTS.push_back("MyProjects/HFM/Instance/Mokoko/caseMay/completeWS");
    completeTS.push_back("MyProjects/HFM/Instance/Mokoko/caseMay/completeLoad");
    completeTS.push_back("MyProjects/HFM/Instance/Mokoko/caseMay/completeD");

    treatForecasts rFComplete(completeTS);

    string trainSotaventoLoad =
        "MyProjects/HFM/Instance/Mokoko/caseMay/trainSotaventoLoad";
    string trainSotaventoD =
        "MyProjects/HFM/Instance/Mokoko/caseMay/trainSotaventoD";
    string trainSotaventoWS =
        "MyProjects/HFM/Instance/Mokoko/caseMay/trainSotaventoWS";

    string valSotaventoLoad =
        "MyProjects/HFM/Instance/Mokoko/caseMay/valSotaventoLoad";
    string valSotaventoD =
        "MyProjects/HFM/Instance/Mokoko/caseMay/valSotaventoD";
    string valSotaventoWS =
        "MyProjects/HFM/Instance/Mokoko/caseMay/valSotaventoWS";

    vector<string> trainTS;
    trainTS.push_back(trainSotaventoLoad);
    trainTS.push_back(trainSotaventoWS);
    trainTS.push_back(trainSotaventoD);
    treatForecasts rFTrain(trainTS);

    vector<string> valTS;
    valTS.push_back(valSotaventoLoad);
    valTS.push_back(valSotaventoWS);
    valTS.push_back(valSotaventoD);
    treatForecasts rFVal(valTS);

    int nTotalForecastingsTrainningSet = rFTrain.getForecastsSize(0);

    //========SET PROBLEM MAXIMUM LAG ===============
    cout << "argvMaxLagRate = " << argvMaxLagRate << endl;

    int iterationMaxLag =
        ((nTotalForecastingsTrainningSet - stepsAhead) * argvMaxLagRate) /
        100.0;
    iterationMaxLag = ceil(iterationMaxLag);
    if (iterationMaxLag > (nTotalForecastingsTrainningSet - stepsAhead))
      iterationMaxLag--;
    if (iterationMaxLag <= 0) iterationMaxLag = 1;

    problemParam.setMaxLag(iterationMaxLag);
    problemParam.setMaxLag(50);  // fixed for a fair comparison
    int maxLag = problemParam.getMaxLag(0);

    // If maxUpperLag is greater than 0 model uses predicted data
    problemParam.setMaxUpperLag(0);
    // int maxUpperLag = problemParam.getMaxUpperLag();
    //=================================================

    vector<double> foIndicators;

    cout << "#nTotalForecastingsTrainningSet: "
         << nTotalForecastingsTrainningSet << endl;

    cout << "#sizeTrainingSet: " << rFTrain.getForecastsSize(0) << endl;
    cout << "#maxNotUsed: " << maxLag << endl;
    cout << "#StepsAhead: " << stepsAhead << endl << endl;

    int nTrainingDays = 30;
    int nTrainingSamples = maxLag + nTrainingDays * stepsAhead;  // 1176
    //		nTrainingSamples = 1176;
    double NTRaprox = (nTrainingSamples - maxLag) / double(stepsAhead);
    cout << "#~NTR: " << NTRaprox << endl;

    int nValidationRounds = 0;
    vector<double> predictedValues;

    int nSamplesComplete = rFComplete.getForecastsSize(0);
    cout << "running test with:" << nSamplesComplete << " samples" << endl;
    for (int begin = 1176; (begin + nSA) <= nSamplesComplete; begin += nSA) {
      cout << "=========================================" << endl;
      cout << "Executing train and forecasts for round: " << nValidationRounds
           << endl;
      nValidationRounds++;
      vector<vector<double>> trainningSet;  // trainningSetVector
      for (int nEXP = 0; nEXP < argvNEXP; nEXP++)
        trainningSet.push_back(rFComplete.getPartsForecastsBeginToEnd(
            nEXP, begin - nTrainingSamples, nTrainingSamples));

      ForecastClass forecastObject(trainningSet, problemParam, rg, methodParam);

      std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;
      int timeES = argvTimeES;  // online training time
      sol = forecastObject.run(timeES, 0, 0);

      vector<vector<double>> validationSet;
      for (int nEXP = 0; nEXP < argvNEXP; nEXP++)
        validationSet.push_back(rFComplete.getPartsForecastsBeginToEnd(
            nEXP, begin - maxLag, nSA + maxLag));

      vector<double> obtainedForecasts =
          *forecastObject.returnForecasts(*sol, validationSet);
      for (int fh = 0; fh < nSA; fh++)
        predictedValues.push_back(obtainedForecasts[fh]);
      cout << predictedValues << endl;
      //			getchar();
      //			avgErrors[MMAPE_INDEX] += errors[MMAPE_INDEX];
      //			avgErrors[RMSE_INDEX] += errors[RMSE_INDEX];

      //			vector<double> targetValuesForPersistance =
      //rFComplete.getPartsForecastsBeginToEnd(0, begin + nTrainingSamples - 1,
      //nSA + 1); 			vector<double> persistanceMethodErrors =
      //forecastObject.returnErrorsPersistance(targetValuesForPersistance, nSA);
      //
      //			avgErrorsP[MMAPE_INDEX] +=
      //persistanceMethodErrors[MMAPE_INDEX]; 			avgErrorsP[RMSE_INDEX] +=
      //persistanceMethodErrors[RMSE_INDEX];

      //			cout << "Round: " << nRounds << " HFM errors
      //MMAPE and RMSE:" << endl; 			cout << errors[MMAPE_INDEX] << "\t" <<
      //errors[RMSE_INDEX] << endl;
      cout << "=========================================" << endl << endl;
    }
    cout << "nRounds = " << nValidationRounds << endl;
    //		getchar();
    //		avgErrors[MMAPE_INDEX] /= nRounds;
    //		avgErrors[RMSE_INDEX] /= nRounds;
    //		avgErrorsP[MMAPE_INDEX] /= nRounds;
    //		avgErrorsP[RMSE_INDEX] /= nRounds;

    //		vector<vector<double> > trainningSet; // trainningSetVector
    //		for (int nEXP = 0; nEXP < argvNEXP; nEXP++)
    //			trainningSet.push_back(rFTrain.getPartsForecastsEndToBegin(nEXP,
    //0, nTotalForecastingsTrainningSet));
    //
    //		cout << trainningSet << endl;
    //		ForecastClass forecastObject(trainningSet, problemParam, rg,
    //methodParam);
    //
    //		pair<Solution<RepEFP>&, Evaluation<>&>* sol;
    //		int timeES = argvTimeES; // online training time
    //		sol = forecastObject.run(timeES, 0, 0);
    ////		cout << sol->first.getR() << endl;
    ////		getchar();
    //
    //		vector<double> foIndicatorCalibration;
    //		vector<vector<double> > validationSet;
    //		for (int nEXP = 0; nEXP < argvNEXP; nEXP++)
    //			validationSet.push_back(rFVal.getPartsForecastsEndToBegin(nEXP,
    //0, rFVal.getForecastsSize(0)));
    //
    //		cout << validationSet << endl;
    //		vector<double> errors = forecastObject.returnErrors(sol,
    //validationSet);
    //

    vector<vector<double>> notUsedSet;
    for (int nEXP = 0; nEXP < argvNEXP; nEXP++)
      notUsedSet.push_back(rFComplete.getPartsForecastsEndToBegin(
          nEXP, 0, nValidationRounds * nSA));
    ForecastClass forecastingClassOBJ(notUsedSet, problemParam, rg,
                                      methodParam);

    vector<double> persistanceMethodErrors =
        *forecastingClassOBJ.returnErrorsPersistance(
            rFComplete.getPartsForecastsEndToBegin(0, 0,
                                                   nValidationRounds * nSA + 1),
            nSA);
    cout << "persistanceMethod errors MMAPE and RMSE:" << endl;
    cout << persistanceMethodErrors[MMAPE_INDEX] << "\t"
         << persistanceMethodErrors[RMSE_INDEX] << endl;
    //

    vector<double> targetValues =
        rFComplete.getPartsForecastsEndToBegin(0, 0, nValidationRounds * nSA);
    vector<double> errors =
        *forecastingClassOBJ.callEvalGetAccuracy(targetValues, predictedValues);
    cout << "HFM errors MMAPE and RMSE:" << endl;
    cout << errors[MMAPE_INDEX] << "\t" << errors[RMSE_INDEX] << endl;

    foIndicators.push_back(errors[MMAPE_INDEX]);
    foIndicators.push_back(errors[RMSE_INDEX]);
    //		foIndicators.push_back(sol->second.evaluation());
    foIndicators.push_back(argvNEXP);
    foIndicators.push_back(maxLag);
    foIndicators.push_back(argvTimeES);
    foIndicators.push_back(nTrainingDays);
    foIndicators.push_back(NTRaprox);
    foIndicators.push_back(contructiveNumberOfRules);
    foIndicators.push_back(alphaACF);
    foIndicators.push_back(argvMaxLagRate);

    foIndicators.push_back(seed);
    vfoIndicatorCalibration.push_back(foIndicators);
  }

  cout << setprecision(3);

  // =================== PRINTING RESULTS ========================
  cout << "\n Printing obtained results for each batch..." << endl;
  for (int n = 0; n < nBatches; n++) {
    for (int i = 0; i < int(vfoIndicatorCalibration[n].size()); i++)
      cout << vfoIndicatorCalibration[n][i] << "\t";

    cout << endl;
  }
  // =======================================================

  // =================== PRINTING RESULTS ON FILE ========================
  string calibrationFile = "./MokokoWindSotavento";

  FILE* fResults = fopen(calibrationFile.c_str(), "a");
  for (int n = 0; n < nBatches; n++) {
    for (int i = 0; i < int(vfoIndicatorCalibration[n].size()); i++)
      fprintf(fResults, "%.7f\t", vfoIndicatorCalibration[n][i]);
    fprintf(fResults, "\n");
  }

  fclose(fResults);
  // =======================================================

  return 0;
}
