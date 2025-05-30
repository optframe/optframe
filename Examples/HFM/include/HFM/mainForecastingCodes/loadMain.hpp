// ===================================
// Main.cpp file generated by OptFrame
// Project EFP
// ===================================

#include <math.h>
#include <stdlib.h>

#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>
#include <iostream>

#include "../ForecastClass.hpp"

using namespace std;
using namespace optframe;
using namespace HFM;

int loadCompetitionBlind(int argc, char** argv) {
  std::cout << "Welcome to Price Competition Mode..." << std::endl;
  RandGenMersenneTwister rg;
  sref<RandGen> rg2{new RandGenMersenneTwister{}};
  // long seed = time(nullptr);
  long seed = 1;
  std::cout << "Seed = " << seed << std::endl;
  srand(seed);
  rg.setSeed(seed);

  if (argc != 10) {
    std::cout << "Parametros incorretos!" << std::endl;
    std::cout << "Os parametros esperados sao: nome nomeValidationSet saida "
                 "parameters options precision"
              << std::endl;
    exit(1);
  }

  const char* caminho = argv[1];
  const char* caminhoValidation = argv[2];
  const char* caminhoOutput = argv[3];
  const char* caminhoParameters = argv[4];
  int instN = atoi(argv[5]);
  int stepsAhead = atoi(argv[6]);
  int mu = atoi(argv[7]);

  string nome = caminho;
  string nomeValidationSet = caminhoValidation;
  string nomeOutput = caminhoOutput;
  string parametersFile = caminhoParameters;

  //===================================
  std::cout << "Parametros:" << std::endl;
  std::cout << "nome=" << nome << std::endl;
  std::cout << "nomeValidationSet=" << nomeValidationSet << std::endl;
  std::cout << "nomeOutput=" << nomeOutput << std::endl;
  std::cout << "nomeParameters=" << parametersFile << std::endl;
  std::cout << "instN=" << instN << std::endl;
  std::cout << "stepsAhead=" << stepsAhead << std::endl;
  std::cout << "mu=" << mu << std::endl;
  //===========================================

  // parametersFile = "./MyProjects/configParametersDay";
  vector<string> vParametersFiles;
  // vParametersFiles.push_back("./MyProjects/configParametersDay");
  vParametersFiles.push_back("./MyProjects/configParametersLoadComp1");

  vector<string> explanatoryVariables;

  string col1 = "./MyProjects/EFP/Instance/LoadCompetition/task7Col1";
  string col2 = "./MyProjects/EFP/Instance/LoadCompetition/Task5/task5Col2";
  string col3 = "./MyProjects/EFP/Instance/LoadCompetition/Task5/task5Col3";
  string col4 = "./MyProjects/EFP/Instance/LoadCompetition/Task5/task5Col4";
  explanatoryVariables.push_back(col1);
  explanatoryVariables.push_back(col2);
  explanatoryVariables.push_back(col3);
  explanatoryVariables.push_back(col4);

  treatForecasts rF(explanatoryVariables);

  // FOR OFFLINE PARAMETERS CALIBRATION
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(0, 24,
  // nTotalForecastings));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(1, 24,
  // nTotalForecastings + stepsAhead));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(2, 24,
  // nTotalForecastings + stepsAhead));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(0, 2000,
  // nTotalForecastings));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(1, 2000,
  // nTotalForecastings + stepsAhead));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(2, 2000,
  // nTotalForecastings + stepsAhead));

  vector<vector<double>> batchOfBlindResults;
  vector<vector<double>> tForecastings;

  /*
   //TESTE VERIFICAR FUNCAO PINBALL
   vector<vector<double> > benchmarkTask2 =
   rF.readQuantilesFromFile("./MyProjects/benchmarkTask2",720);
   std::cout<<benchmarkTask2[719]<<endl;
   vector<double> realValueBenchmarkTask2= rF.getPartsForecastsEndToBegin(0, 0,
   720); std::cout<<realValueBenchmarkTask2<<endl;
   std::cout<<rF.getPinball(benchmarkTask2,realValueBenchmarkTask2)<<endl;
   getchar();
   */

  // ============ FORCES ======================
  int evalFOMinimizer = PINBALL_INDEX;
  int evalAprox = 2;
  int randomPrecision = 20;
  //	int randomParametersFiles = 0;
  mu = 10;
  int lambda = mu * 6;
  double initialDesv = 1;
  double mutationDesv = 2;
  int construtive = 1;
  // ============ END FORCES ======================

  // ============= METHOD PARAMETERS=================
  HFMParams methodParam;
  // seting up ES params
  methodParam.setESMU(mu);
  methodParam.setESLambda(lambda);
  methodParam.setESInitialDesv(initialDesv);
  methodParam.setESMutationDesv(mutationDesv);
  methodParam.setESMaxG(1000);
  // seting up Construtive params
  methodParam.setConstrutiveMethod(construtive);
  methodParam.setConstrutivePrecision(randomPrecision);
  vector<double> vAlphaACFlimits;
  vAlphaACFlimits.push_back(0.8);
  methodParam.setConstrutiveLimitAlphaACF(vAlphaACFlimits);
  // seting up Eval params
  methodParam.setEvalAprox(evalAprox);
  methodParam.setEvalFOMinimizer(evalFOMinimizer);
  // ==========================================

  // ================== READ FILE ============== CONSTRUTIVE 0 AND 1
  //	ProblemParameters problemParam(vParametersFiles[randomParametersFiles]);
  ProblemParameters problemParam;
  stepsAhead = problemParam.getStepsAhead();
  // =========================================== CONSTRUTIVE 0 AND 1
  //========ADAPTATION FOR CONSTRUTIVE 2 ===============
  if (construtive == 2)  // ACF construtive
    problemParam.setMaxLag(300);
  //=================================================

  int nTrainningRounds = 3;
  int nTotalForecastings =
      (problemParam.getMaxLag(0) + nTrainningRounds * stepsAhead) *
      7;  // because of the weeks
  int samplesToBeForecasted = 720;
  int nBatches = 1;
  int timeES = 60;
  int timeVND = 60;
  int timeILS = 60;

  for (int n = 0; n < nBatches; n++) {
    tForecastings.clear();
    tForecastings.push_back(
        rF.getPartsForecastsEndToBegin(0, 0, nTotalForecastings));

    // tForecastings.push_back(rF.getLastForecasts(0, nTotalForecastings));
    // //For the target file tForecastings.push_back(rF.getLastForecasts(1,
    // nTotalForecastings + stepsAhead)); //the auxiliary files has the
    // stepsAhead of the next day to be forecasted
    // tForecastings.push_back(rF.getLastForecasts(2, nTotalForecastings +
    // stepsAhead)); //the auxiliary files has the stepsAhead of the next day to
    // be forecasted tForecastings.push_back(rF.getPartsForecastsEndToBegin(0, n
    // * (nTotalForecastings / 10), nTotalForecastings));
    // tForecastings.push_back(rF.getPartsForecastsEndToBegin(1, 0,
    // nTotalForecastings));
    // tForecastings.push_back(rF.getPartsForecastsEndToBegin(2, 0,
    // nTotalForecastings));
    // tForecastings.push_back(rF.getPartsForecastsEndToBegin(3, 0,
    // nTotalForecastings));
    // tForecastings.push_back(rF.getPartsForecastsEndToBegin(1, n *
    // (nTotalForecastings / 10), nTotalForecastings + stepsAhead));
    // tForecastings.push_back(rF.getPartsForecastsEndToBegin(2, n *
    // (nTotalForecastings / 10), nTotalForecastings + stepsAhead));

    vector<vector<double>> tForecastDayByDay =
        rF.divideByWeekDays(tForecastings[0]);

    vector<vector<double>> vBlindForecastDayByDay;
    for (int d = 6; d >= 0; d--) {
      tForecastings.clear();
      tForecastings.push_back(tForecastDayByDay[d]);

      ForecastClass pFC{tForecastings, problemParam, rg2, methodParam};

      std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;
      sol = pFC.run(timeES, timeVND, timeILS);
      vector<double> dayBlindForecasts;
      int maxLag = problemParam.getMaxLag(0);
      vector<double> dayValidation =
          rF.getPartsForecastsEndToBegin(tForecastDayByDay[d], 0, maxLag);
      vector<vector<double>> validationBlindForecastings;
      validationBlindForecastings.push_back(dayValidation);

      dayBlindForecasts =
          *pFC.returnBlind(sol->first.getR(), validationBlindForecastings);

      vBlindForecastDayByDay.push_back(dayBlindForecasts);
    }

    vector<double> blindResults =
        rF.mixWeekDays(vBlindForecastDayByDay, samplesToBeForecasted);
    batchOfBlindResults.push_back(blindResults);
  }

  // std::cout << batchOfBlindResults << std::endl;
  // getchar();

  double quantilError = 0.5;
  vector<vector<double>> finalQuantisResultsTask4 =
      rF.generateQuantisProbabilisticVectorFixedQuantilError(
          batchOfBlindResults[0], quantilError);
  rF.exportQuantisVector(finalQuantisResultsTask4,
                         "./ProbResultsLoad/finalSubmissionLoadCompTask7");

  /*
   vector<vector<double> > finalResultQuantis;

   for (int sa = 0; sa < samplesToBeForecasted; sa++)
   {
   vector<double> distribution;
   for (int n = 0; n < nBatches; n++)
   {
   distribution.push_back(batchOfBlindResults[n][sa]);
   }
   vector<double> quantis;
   quantis = rF.generateQuantis(distribution, 1, 99);
   finalResultQuantis.push_back(quantis);
   }

   vector<double> realValues = rF.getPartsForecastsEndToBegin(0, 0,
   samplesToBeForecasted);

   double foPVectors = 1000000;
   for (int n = 0; n < nBatches; n++)
   {
   vector<vector<vector<double> > > vQuantisResults =
   rF.generateQuantisProbabilisticVector(batchOfBlindResults[n]);

   int vQ = 0;
   for (double quantilError = 0; quantilError <= 1; quantilError = quantilError
   + 0.05)
   {
   double foPinballQuantil = rF.getPinball(vQuantisResults[vQ], realValues);
   if (foPinballQuantil < foPVectors)
   {
   foPVectors = foPinballQuantil;
   std::cout << "PINBALL(Quantil = " << quantilError << ") batch = " << n <<
   "\t" << foPinballQuantil << std::endl;
   }
   vQ++;
   }
   }

   std::cout << "MAPE = " << rF.getMape(batchOfBlindResults[0], realValues) <<
   std::endl; std::cout << "PINBALL = " << rF.getPinball(finalResultQuantis,
   realValues) << std::endl;

   std::cout << finalResultQuantis[0] << std::endl;

   //cout << finalResultQuantis << std::endl;
   rF.exportQuantisVector(finalResultQuantis,
   "./ProbResultsLoad/quantilB20_ES4_VND10_TS10");
   */

  return 0;
}

int loadCompetitionCalibrationMode(int argc, char** argv) {
  std::cout << "Welcome to Load Competition Calibration Mode..." << std::endl;
  RandGenMersenneTwister rg;
  sref<RandGen> rg2{new RandGenMersenneTwister{}};
  long seed = time(nullptr);
  std::cout << "Seed = " << seed << std::endl;
  srand(seed);
  rg.setSeed(seed);

  if (argc != 10) {
    std::cout << "Parametros incorretos!" << std::endl;
    std::cout << "Os parametros esperados sao: nome nomeValidationSet saida "
                 "parameters options precision"
              << std::endl;
    exit(1);
  }

  const char* caminho = argv[1];
  const char* caminhoValidation = argv[2];
  const char* caminhoOutput = argv[3];
  const char* caminhoParameters = argv[4];
  int instN = atoi(argv[5]);
  int stepsAhead = atoi(argv[6]);
  int mu = atoi(argv[7]);

  string nome = caminho;
  string nomeValidationSet = caminhoValidation;
  string nomeOutput = caminhoOutput;
  string parametersFile = caminhoParameters;

  //===================================
  std::cout << "Parametros:" << std::endl;
  std::cout << "nome=" << nome << std::endl;
  std::cout << "nomeValidationSet=" << nomeValidationSet << std::endl;
  std::cout << "nomeOutput=" << nomeOutput << std::endl;
  std::cout << "nomeParameters=" << parametersFile << std::endl;
  std::cout << "instN=" << instN << std::endl;
  std::cout << "stepsAhead=" << stepsAhead << std::endl;
  std::cout << "mu=" << mu << std::endl;
  //==========================================

  vector<string> vParametersFiles;
  vParametersFiles.push_back("./MyProjects/configParametersDay");
  vParametersFiles.push_back("./MyProjects/configParametersLoadComp1");

  vector<string> explanatoryVariables;
  string col1 = "./MyProjects/EFP/Instance/LoadCompetition/Task5/task5Col1";
  string col2 = "./MyProjects/EFP/Instance/LoadCompetition/Task5/task5Col2";
  string col3 = "./MyProjects/EFP/Instance/LoadCompetition/Task5/task5Col3";
  string col4 = "./MyProjects/EFP/Instance/LoadCompetition/Task5/task5Col4";
  explanatoryVariables.push_back(col1);
  explanatoryVariables.push_back(col2);
  explanatoryVariables.push_back(col3);
  explanatoryVariables.push_back(col4);

  treatForecasts rF(explanatoryVariables);
  // FOR OFFLINE PARAMETERS CALIBRATION
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(0, 24,
  // nTotalForecastings));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(1, 24,
  // nTotalForecastings + stepsAhead));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(2, 24,
  // nTotalForecastings + stepsAhead));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(0, 2000,
  // nTotalForecastings));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(1, 2000,
  // nTotalForecastings + stepsAhead));
  // tForecastings.push_back(rF.getPartsForecastsEndToBegin(2, 2000,
  // nTotalForecastings + stepsAhead));
  int samplesToBeForecasted = 744;
  int nBatches = 1;

  int maxPrecision = 50;
  int maxTrainningRounds = 10;
  int maxMu = 100;
  int maxInitialDesv = 10;
  int maxMutationDesv = 30;

  int timeES = 1;
  int timeVND = 1;
  int timeILS = 0;

  int initialDesv = rg.rand(maxInitialDesv) + 1;
  int mutationDesv = rg.rand(maxMutationDesv) + 1;
  mu = rg.rand(maxMu) + 1;
  int lambda = mu * 6;
  int evalFOMinimizer = rg.rand(
      EVALUTORS_NMETRICS_ENUM_COUNT);  // tree is the number of possible
                                       // objetive function index minimizers
  int evalAprox = rg.rand(2);  // Enayatifar aproximation using previous values
  int construtive = 0;
  int randomPrecision = rg.rand(maxPrecision) + 10;

  // ============= METHOD PARAMETERS=================
  HFMParams methodParam;
  // seting up ES params
  methodParam.setESMU(mu);
  methodParam.setESLambda(lambda);
  methodParam.setESInitialDesv(initialDesv);
  methodParam.setESMutationDesv(mutationDesv);
  methodParam.setESMaxG(1000);
  // seting up Construtive params
  methodParam.setConstrutiveMethod(construtive);
  methodParam.setConstrutivePrecision(randomPrecision);
  vector<double> vAlphaACFlimits;
  vAlphaACFlimits.push_back(0.8);
  methodParam.setConstrutiveLimitAlphaACF(vAlphaACFlimits);
  // seting up Eval params
  methodParam.setEvalAprox(evalAprox);
  methodParam.setEvalFOMinimizer(evalFOMinimizer);
  // ==========================================

  vector<vector<double>> batchOfBlindResults;

  // vector<SolutionEFP> vSolutionsBatches;
  vector<SolutionHFM> vSolutionsBatches;
  vector<vector<double>> vfoIndicatorCalibration;

  for (int n = 0; n < nBatches; n++) {
    // ================== READ FILE ============== CONSTRUTIVE 0 AND 1
    //		int randomParametersFiles = rg.rand(vParametersFiles.size());
    //		ProblemParameters
    // problemParam(vParametersFiles[randomParametersFiles]); //DEPRECATED
    ProblemParameters problemParam;
    stepsAhead = problemParam.getStepsAhead();
    // =========================================== CONSTRUTIVE 0 AND 1
    //========ADAPTATION FOR CONSTRUTIVE 2 ===============
    if (construtive == 2)  // ACF construtive
      problemParam.setMaxLag(300);
    //=================================================

    int nTrainningRounds = rg.rand(maxTrainningRounds) + 1;
    int nTotalForecastingsTrainningSet =
        (problemParam.getMaxLag(0) + nTrainningRounds * stepsAhead) *
        7;  // because of the weeks
    int beginTrainingSet = rg.rand(rF.getForecastsDataSize());

    while ((beginTrainingSet + nTotalForecastingsTrainningSet >
            rF.getForecastsDataSize()) &&
           (beginTrainingSet + 7 * stepsAhead < rF.getForecastsDataSize())) {
      beginTrainingSet = rg.rand(rF.getForecastsDataSize());
    }

    vector<vector<double>> trainningSetComplete;
    trainningSetComplete.push_back(rF.getPartsForecastsEndToBegin(
        0, beginTrainingSet, nTotalForecastingsTrainningSet));
    // trainningSet.push_back(rF.getPartsForecastsEndToBegin(1,
    // beginTrainingSet, nTotalForecastingsTrainningSet));
    // trainningSet.push_back(rF.getPartsForecastsEndToBegin(2,
    // beginTrainingSet, nTotalForecastingsTrainningSet));

    // tForecastings.push_back(rF.getLastForecasts(0, nTotalForecastings));
    // //For the target file tForecastings.push_back(rF.getLastForecasts(1,
    // nTotalForecastings + stepsAhead)); //the auxiliary files has the
    // stepsAhead of the next day to be forecasted
    // tForecastings.push_back(rF.getLastForecasts(2, nTotalForecastings +
    // stepsAhead)); //the auxiliary files has the stepsAhead of the next day to
    // be forecasted tForecastings.push_back(rF.getPartsForecastsEndToBegin(0, n
    // * (nTotalForecastings / 10), nTotalForecastings));
    // tForecastings.push_back(rF.getPartsForecastsEndToBegin(3, 0,
    // nTotalForecastings));
    // tForecastings.push_back(rF.getPartsForecastsEndToBegin(1, n *
    // (nTotalForecastings / 10), nTotalForecastings + stepsAhead));
    // tForecastings.push_back(rF.getPartsForecastsEndToBegin(2, n *
    // (nTotalForecastings / 10), nTotalForecastings + stepsAhead));

    vector<vector<double>> trainningSetDayByDay =
        rF.divideByWeekDays(trainningSetComplete[0]);
    vector<vector<double>> vBlindForecastDayByDay;

    for (int d = 6; d >= 0; d--) {
      vector<vector<double>> trainningSetDay;
      trainningSetDay.push_back(trainningSetDayByDay[d]);

      vector<vector<double>> validationBlindForecastingsDay;
      vector<double> validationDay = rF.getPartsForecastsEndToBegin(
          trainningSetDayByDay[d], 0, problemParam.getMaxLag(0));
      validationBlindForecastingsDay.push_back(validationDay);
      // validationBlindForecastings.push_back(rF.getLastForecasts(0,
      // problemParam.getNotUsedForTest()));

      ForecastClass pFC{trainningSetDay, problemParam, rg2, methodParam};
      // dayBlindForecasts = priceForecastMainLoopCompetition(tForecastings,
      // validationBlindForecastings, problemParam, rg, mu, lambda, initialDesv,
      // mutationDesv);

      std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;
      sol = pFC.run(timeES, timeVND, timeILS);
      vector<double> dayBlindForecasts;
      dayBlindForecasts =
          *pFC.returnBlind(sol->first.getR(), validationBlindForecastingsDay);

      vBlindForecastDayByDay.push_back(dayBlindForecasts);

      vSolutionsBatches.push_back(sol->first);
    }

    vector<double> blindResults =
        rF.mixWeekDays(vBlindForecastDayByDay, samplesToBeForecasted);
    batchOfBlindResults.push_back(blindResults);
  }

  // std::cout << batchOfBlindResults << std::endl;
  // getchar();
  vector<vector<double>> finalResultQuantis;

  for (int sa = 0; sa < samplesToBeForecasted; sa++) {
    vector<double> distribution;
    for (int n = 0; n < nBatches; n++) {
      distribution.push_back(batchOfBlindResults[n][sa]);
    }
    vector<double> quantis;
    quantis = rF.generateQuantis(distribution, 1, 99);
    finalResultQuantis.push_back(quantis);
  }

  vector<double> realValues =
      rF.getPartsForecastsEndToBegin(0, 0, samplesToBeForecasted);

  double foPVectors = 1000000;
  for (int n = 0; n < nBatches; n++) {
    vector<vector<vector<double>>> vQuantisResults =
        rF.generateQuantisProbabilisticVector(batchOfBlindResults[n]);

    int vQ = 0;
    for (double quantilError = 0; quantilError <= 1;
         quantilError = quantilError + 0.05) {
      double foPinballQuantil = rF.getPinball(vQuantisResults[vQ], realValues);
      if (foPinballQuantil < foPVectors) {
        foPVectors = foPinballQuantil;
        std::cout << "PINBALL(Quantil = " << quantilError << ") batch = " << n
                  << "\t" << foPinballQuantil << std::endl;
      }
      vQ++;
    }
  }

  std::cout << "MAPE = " << rF.getMape(batchOfBlindResults[0], realValues)
            << std::endl;
  std::cout << "PINBALL = " << rF.getPinball(finalResultQuantis, realValues)
            << std::endl;

  std::cout << finalResultQuantis[0] << std::endl;

  // std::cout << finalResultQuantis << std::endl;
  rF.exportQuantisVector(finalResultQuantis,
                         "./ProbResultsLoad/quantilB20_ES4_VND10_TS10");

  vector<vector<double>> finalQuantisResultsTask4 =
      rF.generateQuantisProbabilisticVectorFixedQuantilError(
          batchOfBlindResults[0], 0.5);
  rF.exportQuantisVector(finalQuantisResultsTask4,
                         "./ProbResults/finalSubmissionLoadCompTask4");
  return 0;
}
