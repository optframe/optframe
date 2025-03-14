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

int CIFWCCICalibration(int argc, char** argv) {
  std::cout << "Welcome to WCCI calibration for the competition!" << std::endl;
  RandGenMersenneTwister rg;
  // long  1412730737
  long seed = time(nullptr);  // CalibrationMode
  // seed = 9;
  std::cout << "Seed = " << seed << std::endl;
  srand(seed);
  rg.setSeed(seed);

  if (argc != 5) {
    std::cout << "Parametros incorretos!" << std::endl;
    std::cout << "Os parametros esperados sao: nomeOutput targetTS "
            "construtiveNRulesACF timeES"
         << std::endl;
    exit(1);
  }

  const char* caminhoOutput = argv[1];
  int argvTargetTimeSeries = atoi(argv[2]);
  int argvMaxLagRate = atoi(argv[3]);
  int argvTimeES = atoi(argv[4]);

  string nomeOutput = caminhoOutput;
  //===================================
  std::cout << "Parametros:" << std::endl;
  std::cout << "nomeOutput=" << nomeOutput << std::endl;
  std::cout << "argvTargetTimeSeries=" << argvTargetTimeSeries << std::endl;
  std::cout << "argvMaxLagRate=" << argvMaxLagRate << std::endl;
  std::cout << "argvTimeES=" << argvTimeES << std::endl;

  File fileWCCIInstances{"./MyProjects/HFM/Instance/WCCI/CIFInstances"};

  if (!fileWCCIInstances.isOpen()) {
    std::cout << "File '" << "./MyProjects/HFM/Instance/WCCI/CIFInstances"
         << "' not found" << std::endl;
    exit(1);
  }

  Scanner* scannerWCCI = new Scanner{std::move(fileWCCIInstances)};
  for (int i = 0; i < (argvTargetTimeSeries - 1); i++) scannerWCCI->nextLine();

  string testProblemWCCI = scannerWCCI->nextLine();
  delete scannerWCCI;
  scannerWCCI = new Scanner(testProblemWCCI);

  // TODO: must ignore next line?
  std::cout << *scannerWCCI->nextInt() << std::endl;
  int WCCIStepsAhead = *scannerWCCI->nextInt();
  std::cout << "Required steps ahead:" << WCCIStepsAhead << std::endl;

  vector<double> forecastsTSWCCI;
  while (scannerWCCI->hasNext())
    forecastsTSWCCI.push_back(*scannerWCCI->nextDouble());
  std::cout << forecastsTSWCCI << std::endl;

  vector<vector<double>> forecastingWCCIExogenousVariables;
  forecastingWCCIExogenousVariables.push_back(forecastsTSWCCI);

  treatForecasts rF(forecastingWCCIExogenousVariables);

  int nBatches = 1;

  vector<vector<double>>
      vfoIndicatorCalibration;  // vector with the FO of each batch

  vector<SolutionEFP>
      vSolutionsBatches;  // vector with the solution of each batch

  for (int n = 0; n < nBatches; n++) {
    //		int contructiveNumberOfRules = rg.rand(maxPrecision) + 10;
    //		int evalFOMinimizer = rg.rand(NMETRICS); //tree is the number of
    //possible objetive function index minimizers 		int evalAprox = rg.rand(2);
    ////Enayatifar aproximation using previous values 		int construtive =
    //rg.rand(3); 		double initialDesv = rg.rand(maxInitialDesv) + 1; 		double
    //mutationDesv = rg.rand(maxMutationDesv) + 1; 		int mu = rg.rand(maxMu) + 1;
    //		int lambda = mu * 6;

    // limit ACF for construtive ACF
    //		double alphaACF = rg.rand01();
    //		int alphaSign = rg.rand(2);
    //		if (alphaSign == 0)
    //			alphaACF = alphaACF * -1;

    // ============ FORCES ======================
    //		initialDesv = 10;
    //		mutationDesv = 20;
    int mu = 100;
    int lambda = mu * 6;
    int evalFOMinimizer = SMAPE_INDEX;
    int contructiveNumberOfRules = 100;
    int evalAprox = 2;
    double alphaACF = 0.5;
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
    int nSA = WCCIStepsAhead;
    problemParam.setStepsAhead(nSA);
    int stepsAhead = problemParam.getStepsAhead();

    int nTotalForecastingsTrainningSet = rF.getForecastsSize(0) - stepsAhead;

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
    trainningSet.push_back(rF.getPartsForecastsEndToBegin(
        0, stepsAhead, nTotalForecastingsTrainningSet));

    ForecastClass forecastObject(trainningSet, problemParam, rg, methodParam);

    //		forecastObject.runMultiObjSearch();
    //		getchar();
    std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;
    sol = forecastObject.run(timeES, 0, 0);

    vector<double> foIndicatorCalibration;
    vector<vector<double>> validationSet;
    validationSet.push_back(
        rF.getPartsForecastsEndToBegin(0, 0, maxLag + stepsAhead));
    vector<double> errors =
        *forecastObject.returnErrors(sol->first.getR(), validationSet);

    foIndicators.push_back(errors[SMAPE_INDEX]);
    foIndicators.push_back(sol->second.evaluation());
    foIndicators.push_back(argvTargetTimeSeries);
    foIndicators.push_back(argvMaxLagRate);
    foIndicators.push_back(maxLag);
    foIndicators.push_back(NTRaprox);
    foIndicators.push_back(timeES);
    foIndicators.push_back(seed);
    vfoIndicatorCalibration.push_back(foIndicators);
  }

  std::cout << setprecision(3);

  //	double averageError = 0;
  //	for (int t = 0; t < (vfoIndicatorCalibration[0].size() - 1); t++)
  //	{
  //		averageError += vfoIndicatorCalibration[0][t];
  //	}
  //
  //	averageError /= (vfoIndicatorCalibration[0].size() - 1);
  //	vfoIndicatorCalibration[0].push_back(averageError);
  // =================== PRINTING RESULTS ========================
  for (int n = 0; n < nBatches; n++) {
    for (int i = 0; i < int(vfoIndicatorCalibration[n].size()); i++)
      std::cout << vfoIndicatorCalibration[n][i] << "\t";

    std::cout << std::endl;
  }
  // =======================================================

  // =================== PRINTING RESULTS ON FILE ========================
  string calibrationFile = "./WCCI_CFI_Calibration";

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

int CIFWCCIGeneratingForecasts(int argc, char** argv) {
  std::cout << "Welcome to WCCI forecasting competition! With this code we "
          "generated the forecasts."
       << std::endl;
  RandGenMersenneTwister rg;
  // long  1412730737
  long seed = time(nullptr);  // CalibrationMode
  // seed = 9;
  std::cout << "Seed = " << seed << std::endl;
  srand(seed);
  rg.setSeed(seed);

  if (argc != 5) {
    std::cout << "Parametros incorretos!" << std::endl;
    std::cout << "Os parametros esperados sao: nomeOutput targetTS "
            "construtiveNRulesACF timeES"
         << std::endl;
    exit(1);
  }

  const char* caminhoOutput = argv[1];
  int argvTargetTimeSeries = atoi(argv[2]);
  int argvMaxLagRate = atoi(argv[3]);
  int argvTimeES = atoi(argv[4]);

  std::cout << "================================" << std::endl;
  std::cout << "forcing MaxLagRate to 40% and timeTraining to 20s" << std::endl;
  argvMaxLagRate = 40;
  argvTimeES = 20;
  std::cout << "================================" << std::endl;

  string nomeOutput = caminhoOutput;
  //===================================
  std::cout << "Parametros:" << std::endl;
  std::cout << "nomeOutput=" << nomeOutput << std::endl;
  std::cout << "argvTargetTimeSeries=" << argvTargetTimeSeries << std::endl;
  std::cout << "argvMaxLagRate=" << argvMaxLagRate << std::endl;
  std::cout << "argvTimeES=" << argvTimeES << std::endl;

  File fileWCCIInstances{"./MyProjects/HFM/Instance/WCCI/CIFInstances"};

  if (!fileWCCIInstances.isOpen()) {
    std::cout << "File '" << "./MyProjects/HFM/Instance/WCCI/CIFInstances"
         << "' not found" << std::endl;
    exit(1);
  }

  Scanner* scannerWCCI = new Scanner{std::move(fileWCCIInstances)};
  for (int i = 0; i < (argvTargetTimeSeries - 1); i++) scannerWCCI->nextLine();

  string testProblemWCCI = scannerWCCI->nextLine();
  delete scannerWCCI;
  scannerWCCI = new Scanner(testProblemWCCI);

  // TODO: should ignore next line?
  std::cout << *scannerWCCI->nextInt() << std::endl;
  int WCCIStepsAhead = *scannerWCCI->nextInt();
  std::cout << "Required steps ahead:" << WCCIStepsAhead << std::endl;

  vector<double> forecastsTSWCCI;
  while (scannerWCCI->hasNext())
    forecastsTSWCCI.push_back(*scannerWCCI->nextDouble());
  std::cout << forecastsTSWCCI << std::endl;

  vector<vector<double>> forecastingWCCIExogenousVariables;
  forecastingWCCIExogenousVariables.push_back(forecastsTSWCCI);

  treatForecasts rF(forecastingWCCIExogenousVariables);

  int nBatches = 1;

  vector<vector<double>>
      vfoIndicatorCalibration;  // vector with the FO of each batch

  vector<SolutionEFP>
      vSolutionsBatches;  // vector with the solution of each batch

  for (int n = 0; n < nBatches; n++) {
    //		int contructiveNumberOfRules = rg.rand(maxPrecision) + 10;
    //		int evalFOMinimizer = rg.rand(NMETRICS); //tree is the number of
    //possible objetive function index minimizers 		int evalAprox = rg.rand(2);
    ////Enayatifar aproximation using previous values 		int construtive =
    //rg.rand(3); 		double initialDesv = rg.rand(maxInitialDesv) + 1; 		double
    //mutationDesv = rg.rand(maxMutationDesv) + 1; 		int mu = rg.rand(maxMu) + 1;
    //		int lambda = mu * 6;

    // limit ACF for construtive ACF
    //		double alphaACF = rg.rand01();
    //		int alphaSign = rg.rand(2);
    //		if (alphaSign == 0)
    //			alphaACF = alphaACF * -1;

    // ============ FORCES ======================
    //		initialDesv = 10;
    //		mutationDesv = 20;
    int mu = 100;
    int lambda = mu * 6;
    int evalFOMinimizer = SMAPE_INDEX;
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
    int nSA = WCCIStepsAhead;
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

    ForecastClass forecastObject(trainningSet, problemParam, rg, methodParam);

    //		forecastObject.runMultiObjSearch();
    //		getchar();
    std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;
    sol = forecastObject.run(timeES, 0, 0);

    vector<double> foIndicatorCalibration;
    vector<vector<double>> dataForBlindForecasts;
    dataForBlindForecasts.push_back(
        rF.getPartsForecastsEndToBegin(0, 0, maxLag));
    vector<double>* blindForecasts =
        forecastObject.returnBlind(sol->first.getR(), dataForBlindForecasts);

    std::cout << blindForecasts << std::endl;

    string resultsWCCIComp = "./WCCI_CFI_FinalResults";

    FILE* fResults = fopen(resultsWCCIComp.c_str(), "a");
    fprintf(fResults, "ts%d", argvTargetTimeSeries);
    for (int n = 0; n < stepsAhead; n++) {
      fprintf(fResults, ";%f", blindForecasts->at(n));
    }
    fprintf(fResults, "\n");

    //		foIndicators.push_back(sol->second.evaluation());
    //		foIndicators.push_back(argvTargetTimeSeries);
    //		foIndicators.push_back(argvMaxLagRate);
    //		foIndicators.push_back(maxLag);
    //		foIndicators.push_back(NTRaprox);
    //		foIndicators.push_back(timeES);
    //		foIndicators.push_back(seed);
    vfoIndicatorCalibration.push_back(foIndicators);
  }

  std::cout << setprecision(3);

  //	double averageError = 0;
  //	for (int t = 0; t < (vfoIndicatorCalibration[0].size() - 1); t++)
  //	{
  //		averageError += vfoIndicatorCalibration[0][t];
  //	}
  //
  //	averageError /= (vfoIndicatorCalibration[0].size() - 1);
  //	vfoIndicatorCalibration[0].push_back(averageError);
  // =================== PRINTING RESULTS ========================
  for (int n = 0; n < nBatches; n++) {
    for (int i = 0; i < int(vfoIndicatorCalibration[n].size()); i++)
      std::cout << vfoIndicatorCalibration[n][i] << "\t";

    std::cout << std::endl;
  }
  // =======================================================

  // =================== PRINTING RESULTS ON FILE ========================
  string calibrationFile = "./WCCI_CFI_Calibration";

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
//
// for (int w = 4; w >= 1; w--)
//	{
//		vector<double> foIndicatorsMAPE;
//		vector<double> foIndicatorsRMSE;
//
//		for (int day = 1; day <= 7; day++)
//		{
//			vector<vector<double> > validationSet; //validation set
//for calibration 			validationSet.push_back(rF.getPartsForecastsEndToBegin(0, w *
//168 - stepsAhead * day, nValidationSamples)); 			vector<double> foIndicators;
//			foIndicators = forecastObject.returnErrors(sol,
//validationSet); 			foIndicatorsMAPE.push_back(foIndicators[MAPE_INDEX]);
//			foIndicatorsRMSE.push_back(foIndicators[RMSE_INDEX]);
//		}
//		double sumMAPE = accumulate(foIndicatorsMAPE.begin(),
//foIndicatorsMAPE.end(), 0.0); 		double sumRMSE =
//accumulate(foIndicatorsRMSE.begin(), foIndicatorsRMSE.end(), 0.0);
//
//		foIndicatorCalibration.push_back(sumMAPE/foIndicatorsMAPE.size());
//		foIndicatorCalibration.push_back(sumRMSE/foIndicatorsRMSE.size());
//	}
