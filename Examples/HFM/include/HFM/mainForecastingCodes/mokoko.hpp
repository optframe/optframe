// ===================================
// Main.cpp file generated by OptFrame
// Project EFP
// ===================================

#include <math.h>
#include <stdlib.h>

#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>
#include <iostream>

using namespace std;
using namespace optframe;
using namespace HFM;

int mokokoProbabilisticForecastWindPower(int argc, char** argv) {
  std::cout << "Welcome to Mokoko's place" << std::endl;
  RandGenMersenneTwister rg;

  long seed = time(nullptr);
  seed = 1;  // Fix the seed
  std::cout << "Seed = " << seed << std::endl;
  srand(seed);
  rg.setSeed(seed);

  //=================================================
  // This comment part can be used for automatic generation of results
  //	if (argc != 10)
  //	{
  //		cout << "Parametros incorretos!" << std::endl;
  //		cout << "Os parametros esperados sao: nome nomeValidationSet
  //saida parameters options precision" << std::endl; 		exit(1);
  //	}

  //	const char* caminho = argv[1];
  //	const char* caminhoValidation = argv[2];
  //	const char* caminhoOutput = argv[3];
  //	const char* caminhoParameters = argv[4];
  //	int instN = atoi(argv[5]);
  //	int stepsAhead = atoi(argv[6]);
  //	int mu = atoi(argv[9]);
  //
  //	string nome = caminho;
  //	string nomeValidationSet = caminhoValidation;
  //	string nomeOutput = caminhoOutput;
  //	string parametersFile = caminhoParameters;

  //===================================
  //	cout << "Parametros:" << std::endl;
  //	cout << "nome=" << nome << std::endl;
  //	cout << "nomeValidationSet=" << nomeValidationSet << std::endl;
  //	cout << "nomeOutput=" << nomeOutput << std::endl;
  //	cout << "nomeParameters=" << parametersFile << std::endl;
  //	cout << "instN=" << instN << std::endl;
  //	cout << "stepsAhead=" << stepsAhead << std::endl;
  //	cout << "mu=" << mu << std::endl;
  //======================================
  //=================================================

  // Mokokos boy - Mokoblaster - This vector contains the time series that will
  // be learned.
  //  The model can receive more than one time series as input.
  //  in this case, the first one is considered as the target to be forecasted
  //  and the other one only as input (end
  vector<string> explanatoryVariables;
  string inputTimeSeries = "./MyProjects/HFM/Instance/Mokoko/1Tri";
  explanatoryVariables.push_back(inputTimeSeries);
  // treatForecasts is the class that read and contains time series parameters
  treatForecasts rF(explanatoryVariables);

  // vector<vector<double> > batchOfBlindResults; //vector with the blind
  // results of each batch

  /*int beginValidationSet = 0;
   int nTrainningRoundsValidation = 50;
   int nValidationSamples = problemParam.getNotUsedForTest() +
   nTrainningRoundsValidation * stepsAhead; std::cout << "nValidationSamples = " <<
   nValidationSamples << std::endl; int nTotalForecastingsValidationSet =
   nValidationSamples;

   vector<vector<double> > validationSet; //validation set for calibration
   validationSet.push_back(rF.getPartsForecastsEndToBegin(0, beginValidationSet,
   nTotalForecastingsValidationSet));
   validationSet.push_back(rF.getPartsForecastsEndToBegin(1, beginValidationSet,
   nTotalForecastingsValidationSet + stepsAhead));
   validationSet.push_back(rF.getPartsForecastsEndToBegin(2, beginValidationSet,
   nTotalForecastingsValidationSet + stepsAhead));
   */

  int maxPrecision = 20;
  int maxTrainningRounds = 1000;
  int timeES = 5;
  int stepsAheadR = 168;

  vector<vector<double>>
      vfoIndicatorCalibration;  // vector with the FO of each batch

  vector<SolutionHFM>
      vSolutionsBatches;  // vector with the solution of each batch
  // Matrix with forecasts of each batch of training and validation
  vector<vector<double>> batchOfResults;
  int nBatches = 5;
  for (int n = 0; n < nBatches; n++) {
    int modelInitialNumberOfRules = rg.rand(maxPrecision) + 10;
    // ============ FORCES ======================
    // OPTIMIZATION PARAMETERS
    int mu = 100;
    int lambda = mu * 6;
    // ================================
    // randomPrecision = 100;
    int evalFOMinimizer = MAPE_INDEX;  // You can choose several function to be
                                       // minimized -- check evaluator
    int evalAprox = 0;  // This is an approximation -- Check the evaluator class
                        // and the paper of Enayatafar
    int construtive = 2;  // As standand, construtive 2 is being adopted --
                          // there is another one, but worse results
    int alphaACF =
        0;  // The limit that guides the initial input of the model from [-1,1]
    // ============ END FORCES ======================

    // ============= METHOD PARAMETERS=================
    HFMParams methodParam;

    // seting up ES params
    methodParam.setESMU(mu);
    methodParam.setESLambda(lambda);
    methodParam.setESInitialDesv(
        10);  // used only if Continuous Evolution Strategies is being used
    methodParam.setESMutationDesv(
        10);  // used only if Continuous Evolution Strategies is being used
    methodParam.setESMaxG(100000);
    // seting up Construtive params
    methodParam.setConstrutiveMethod(construtive);
    methodParam.setConstrutivePrecision(modelInitialNumberOfRules);
    vector<double> vAlphaACFlimits;
    vAlphaACFlimits.push_back(alphaACF);
    methodParam.setConstrutiveLimitAlphaACF(vAlphaACFlimits);
    // seting up Eval params
    methodParam.setEvalAprox(evalAprox);
    methodParam.setEvalFOMinimizer(evalFOMinimizer);
    // ==========================================

    // ================== READ FILE ============== CONSTRUTIVE 0 AND 1
    ProblemParameters problemParam;

    problemParam.setStepsAhead(stepsAheadR);
    int stepsAhead = problemParam.getStepsAhead();
    // stepsAhead = 1;
    //  =========================================== CONSTRUTIVE 0 AND 1
    //========ADAPTATION FOR CONSTRUTIVE 2 ===============
    if (construtive == 2)  // ACF construtive
      problemParam.setMaxLag(500);

    int maxNotUsedForTest = problemParam.getMaxLag(0);

    std::cout << "maxNotUsedForTest: " << maxNotUsedForTest << std::endl;

    // getchar();
    // int randomObjFunc = rg.rand(2);
    // std::cout<<"randomObjFunc = "<<randomObjFunc<<endl;
    // problemParam.setFunction(randomObjFunc);

    // validationBlindForecastings.clear();

    int nTrainningRounds = rg.rand(maxTrainningRounds) + 1;
    nTrainningRounds = 1;
    int nTotalForecastingsTrainningSet =
        maxNotUsedForTest + nTrainningRounds * stepsAhead;
    std::cout << "nTrainningRounds: " << nTrainningRounds << std::endl;
    std::cout << "nTotalForecastingsTrainningSet: " << nTotalForecastingsTrainningSet
         << std::endl;

    vector<vector<double>> trainningSet;  // trainningSetVector
    int beginTrainingSet = stepsAhead;
    trainningSet.push_back(rF.getPartsForecastsEndToBegin(
        0, beginTrainingSet, nTotalForecastingsTrainningSet));

    ForecastClass forecastObject(trainningSet, problemParam, rg, methodParam);

    std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;

    sol = forecastObject.run(timeES, 0, 0);

    int maxLag = problemParam.getMaxLag(0);
    // validation set for calibration
    vector<vector<double>> validationSet;
    validationSet.push_back(rF.getLastForecasts(0, maxLag + stepsAhead));
    batchOfResults.push_back(
        *forecastObject.returnForecasts(*sol, validationSet));

    vector<double> foIndicatorCalibration;
    foIndicatorCalibration =
        *forecastObject.returnErrors(sol->first.getR(), validationSet);
    foIndicatorCalibration.push_back(modelInitialNumberOfRules);
    foIndicatorCalibration.push_back(nTrainningRounds);
    foIndicatorCalibration.push_back(beginTrainingSet);
    foIndicatorCalibration.push_back(nTotalForecastingsTrainningSet);
    foIndicatorCalibration.push_back(mu);
    foIndicatorCalibration.push_back(lambda);
    foIndicatorCalibration.push_back(timeES);
    foIndicatorCalibration.push_back(evalFOMinimizer);
    foIndicatorCalibration.push_back(evalAprox);
    foIndicatorCalibration.push_back(construtive);
    foIndicatorCalibration.push_back(alphaACF);
    foIndicatorCalibration.push_back(seed);
    // getchar();
    // std::cout << foIndicatorCalibration << std::endl;
    vfoIndicatorCalibration.push_back(foIndicatorCalibration);
    vSolutionsBatches.push_back(sol->first);
  }

  std::cout << "Models has been trained with success!" << std::endl;

  vector<vector<double>> finalResultQuantis;

  for (int sa = 0; sa < stepsAheadR; sa++) {
    vector<double> distribution;
    for (int n = 0; n < nBatches; n++) {
      distribution.push_back(batchOfResults[n][sa]);
    }
    vector<double> quantis;
    quantis = rF.generateQuantis(distribution, 1, 99);
    finalResultQuantis.push_back(quantis);
  }

  rF.exportQuantisVector(finalResultQuantis, "./ExportQuantileMokoko");
  rF.exportVectorOfVector(batchOfResults, "./MatrixOfBatchesAndForecasts");
  std::cout << "Pinball Function of Probabilistic Forecasting: \t"
       << rF.getPinball(finalResultQuantis, rF.getLastForecasts(0, stepsAheadR))
       << std::endl;

  std::cout << vfoIndicatorCalibration << std::endl;
  for (int n = 0; n < nBatches; n++) {
    for (int i = 0; i < (int)vfoIndicatorCalibration[n].size(); i++)
      std::cout << vfoIndicatorCalibration[n][i] << "\t";

    std::cout << std::endl;
  }

  string calibrationFile = "./mokokosPlaceAggregatedResults";

  FILE* fResults = fopen(calibrationFile.c_str(), "a");
  for (int n = 0; n < nBatches; n++) {
    for (int i = 0; i < (int)vfoIndicatorCalibration[n].size(); i++)
      fprintf(fResults, "%.7f\t", vfoIndicatorCalibration[n][i]);
    fprintf(fResults, "\n");
  }

  fclose(fResults);

  return 0;
}
