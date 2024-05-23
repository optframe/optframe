/*
 * ForecastClass.hpp
 *
 *  Created on: 10/09/2014
 *      Author: vitor
 */

#ifndef EXAMPLES_HFM_INCLUDE_HFM_FORECASTCLASS_HPP_
#define EXAMPLES_HFM_INCLUDE_HFM_FORECASTCLASS_HPP_

// C++
#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/Heuristics/LocalSearches/VND.h>

#include <OptFrame/GeneralEvaluator.hpp>
#include <OptFrame/Heuristics/EA/NGES.hpp>
#include <OptFrame/Heuristics/Empty.hpp>
#include <OptFrame/Heuristics/GRASP/BasicGRASP.hpp>
#include <OptFrame/Heuristics/ILS/ILSLPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/ILS/MO/BasicMOILS.hpp>
#include <OptFrame/Heuristics/ILS/MO/MOILSLPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/MO/MOILSLevels.hpp>
#include <OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/RandomDescentMethod.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/Heuristics/MOLocalSearches/GPLS.hpp>
#include <OptFrame/Heuristics/MOLocalSearches/MOBestImprovement.hpp>
#include <OptFrame/Heuristics/MOLocalSearches/MORandomImprovement.hpp>
#include <OptFrame/InitialSearch.hpp>
#include <OptFrame/MultiEvaluator.hpp>
#include <OptFrame/MultiObjSearch.hpp>
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/ParetoManager.hpp>
#include <OptFrame/Util/CheckCommand.hpp>

#include "./ConstructiveRandom.hpp"
#include "./Evaluation.h"
#include "./Evaluator.hpp"
#include "./HFMESContinous.hpp"
#include "./HFMParams.hpp"
#include "./MultiEvaluatorHFM.hpp"
#include "./NS/NSSeqHFMAddSingleInput.hpp"
#include "./NS/NSSeqHFMChangeSingleInput.hpp"
#include "./NS/NSSeqHFMModifyRules.hpp"
#include "./NS/NSSeqHFMRemoveSingleInput.hpp"
#include "./NS/NSSeqHFMVAlpha.hpp"
#include "./ProblemInstance.hpp"
#include "./Representation.h"
#include "./Solution.h"
#include "./TreatForecasts.hpp"

namespace HFM {

class ForecastClass {
 private:
  vector<vector<double>>& tForecastings;
  ProblemParameters& problemParam;
  sref<RandGen> rg;
  HFMParams& methodParam;

  HFMProblemInstance* p;
  // HFMEvaluator* eval;
  sref<HFMEvaluator> eval;
  // unused, just redirecting to 'eval'
  sref<GeneralEvaluator<ESolutionHFM>> evalGeneral;

  // Constructive<SolutionHFM>* c;
  sptr<InitialSearch<ESolutionHFM>> c;  // for single obj
  InitialSearch<EMSolutionHFM>* cm;     // for multi obj
  //
  vector<NSSeq<ESolutionHFM>*> vNS;

  EmptyLocalSearch<ESolutionHFM> emptyLS;
  // vector<NSSeq<ESolutionHFM>*>* vNSeq;
  vsref<NSSeq<ESolutionHFM>> vNSeq;
  // vector<NSSeq<EMSolutionHFM, MultiEvaluationHFM>*>* vNSeqMO;
  vsref<NSSeq<EMSolutionHFM>> vNSeqMO;

  // EFPESContinous* EsCOpt;
  sptr<NGES<ESolutionHFM>> es;
  sptr<NGESParams> ngesParams;

  vsref<LocalSearch<ESolutionHFM>> vLS;

  // sptr<VariableNeighborhoodDescent<ESolutionHFM>> vnd;
  sptr<LocalSearch<ESolutionHFM>> vnd;
  sptr<IteratedLocalSearchLevels<ESolutionHFM>> ils;

  vector<vector<double>> vBlindResults;
  vector<pair<SolutionHFM, Evaluation<>>*> vFinalSol;

  sptr<ILSLPerturbation<ESolutionHFM, EvaluationHFM>> ilsPert;
  //
  // MO versions
  // MOILSLPerturbationLPlus2<SolutionHFM>* moILSPert;
  // sptr<MOILSLPerturbationLPlus2<EMSolutionHFM, MultiEvaluationHFM>>
  // moILSPert;
  sptr<MOILSLPerturbation<EMSolutionHFM, MultiEvaluationHFM>> moILSPert;
  //
  // BasicMOILSPerturbation<SolutionHFM>* basicMOILSPert;
  sptr<BasicMOILSPerturbation<EMSolutionHFM, MultiEvaluationHFM>>
      basicMOILSPert;

  // OptimalLinearRegression* olr;
  // MultiEvaluator<RepEFP>* mev;
  sptr<HFMMultiEvaluator> mev;
  sptr<IEvaluator<EMSolutionHFM>> imev;
  CheckCommand<ESolutionHFM, SolutionHFM, RepHFM, OPTFRAME_DEFAULT_ADS>
      checkModule;

 public:
  ForecastClass(vector<vector<double>>& _tForecastings,
                ProblemParameters& _problemParam, sref<RandGen> _rg,
                HFMParams& _methodParam)
      : tForecastings(_tForecastings),
        problemParam(_problemParam),
        rg(_rg),
        methodParam(_methodParam),
        p{new HFMProblemInstance(tForecastings)},
        eval{new HFMEvaluator(*p, problemParam,
                              methodParam.getEvalFOMinimizer(),
                              methodParam.getEvalAprox())},
        evalGeneral{eval} {
    // p = new HFMProblemInstance(tForecastings);
    // eval = new HFMEvaluator(*p, problemParam,
    // methodParam.getEvalFOMinimizer(), methodParam.getEvalAprox());

    // NSSeqHFMModifyRules* nsModifyFuzzyRules = new NSSeqHFMModifyRules(*p,
    // rg);
    //
    // auto nsModifyFuzzyRulesSO = new NSSeqHFMModifyRules<>(*p, rg);
    sref<NSSeq<ESolutionHFM>> nsModifyFuzzyRulesSO =
        new NSSeqHFMModifyRules<>(*p, rg);

    auto* nsModifyFuzzyRulesMO =
        new NSSeqHFMModifyRules<EMSolutionHFM, MultiEvaluationHFM>(*p, rg);

    sref<NSSeq<ESolutionHFM>> nsChangeSingleInput =
        new NSSeqHFMChangeSingleInput<>(*p, rg, problemParam.getVMaxLag(),
                                        problemParam.getVMaxUpperLag());
    auto* nsChangeSingleInputMO =
        new NSSeqHFMChangeSingleInput<EMSolutionHFM, MultiEvaluationHFM>(
            *p, rg, problemParam.getVMaxLag(), problemParam.getVMaxUpperLag());

    sref<NSSeq<ESolutionHFM>> nsRemoveSingleInput =
        new NSSeqHFMRemoveSingleInput<>(rg);
    auto* nsRemoveSingleInputMO =
        new NSSeqHFMRemoveSingleInput<EMSolutionHFM, MultiEvaluationHFM>(rg);

    sref<NSSeq<ESolutionHFM>> nsAddSingleInput = new NSSeqNEIGHAddSingleInput<>(
        *p, rg, problemParam.getVMaxLag(), problemParam.getVMaxUpperLag());
    auto nsAddSingleInputMO =
        new NSSeqNEIGHAddSingleInput<EMSolutionHFM, MultiEvaluationHFM>(
            *p, rg, problemParam.getVMaxLag(), problemParam.getVMaxUpperLag());
    //
    // NSSeqNEIGHVAlpha* nsVAlpha = new NSSeqNEIGHVAlpha(*p, rg, 5);

    // NSSeqNEIGHAddX* nsAddMean01 = new NSSeqNEIGHAddX(*p, rg, 0.1);
    // NSSeqNEIGHAddX* nsAddMean1 = new NSSeqNEIGHAddX(*p, rg, 1);
    // NSSeqNEIGHAddX* nsAddMeanD15 = new NSSeqNEIGHAddX(*p, rg,
    // p->getMean(0) / 15); NSSeqNEIGHAddX* nsAddMeanD6 = new
    // NSSeqNEIGHAddX(*p, rg, p->getMean(0) / 6); NSSeqNEIGHAddX*
    // nsAddMeanD2 = new NSSeqNEIGHAddX(*p, rg, p->getMean(0) / 2);
    // NSSeqNEIGHAddX* nsAddMean = new NSSeqNEIGHAddX(*p, rg, p->getMean(0));
    // NSSeqNEIGHAddX* nsAddMeanM2 = new NSSeqNEIGHAddX(*p, rg, 2 *
    // p->getMean(0)); NSSeqNEIGHAddX* nsAddMeanM5 = new
    // NSSeqNEIGHAddX(*p, rg, 5 * p->getMean(0)); NSSeqNEIGHAddX*
    // nsAddMeanBigM = new NSSeqNEIGHAddX(*p, rg, 100 * p->getMean(0));

    int cMethod = methodParam.getConstrutiveMethod();
    int cPre = methodParam.getConstrutivePrecision();

    if (cMethod == 0) {
      c = sptr<InitialSearch<ESolutionHFM>>{
          new ConstructiveRandom<ESolutionHFM, EvaluationHFM>(*p, problemParam,
                                                              rg, cPre)};
      cm = new ConstructiveRandom<EMSolutionHFM, MultiEvaluationHFM>(
          *p, problemParam, rg, cPre);
    }
    if (cMethod == 2) {
      c = sptr<InitialSearch<ESolutionHFM>>{
          new ConstructiveACF<ESolutionHFM, EvaluationHFM>(
              *p, problemParam, rg, cPre,
              methodParam.getConstrutiveLimitAlphaACF())};
      cm = new ConstructiveACF<EMSolutionHFM, MultiEvaluationHFM>(
          *p, problemParam, rg, cPre,
          methodParam.getConstrutiveLimitAlphaACF());
    }

    // FirstImprovement<SolutionHFM>* fiModifyFuzzyRules = new
    // FirstImprovement<SolutionHFM>(*eval, *nsModifyFuzzyRules);
    // FirstImprovement<SolutionHFM>* fiChangeSingleInput = new
    // FirstImprovement<SolutionHFM>(*eval, *nsChangeSingleInput);
    sref<RandomDescentMethod<ESolutionHFM>> rdmRemove =
        new RandomDescentMethod<ESolutionHFM>(
            eval, nsRemoveSingleInput,
            500);  // FirstImprovement<RepEFP>* fiVAlpha = new
                   //  FirstImprovement<RepEFP>(*eval, *nsVAlpha);
    sref<RandomDescentMethod<ESolutionHFM>> rdmAdd =
        new RandomDescentMethod<ESolutionHFM>(
            eval, nsAddSingleInput,
            500);  // FirstImprovement<RepEFP>* fiVAlpha = new
                   //  FirstImprovement<RepEFP>(*eval, *nsVAlpha);
                   //  int maxRDM = 100;
                   //
                   // //rdm->setMessageLevel(3);
                   // vLS.push_back(fiVAlpha);
    vLS.push_back(rdmRemove);
    vLS.push_back(rdmAdd);
    // vLS.push_back(fiModifyFuzzyRules);
    // vLS.push_back(fiChangeSingleInput);
    vnd = sptr<VariableNeighborhoodDescent<ESolutionHFM>>{
        new VariableNeighborhoodDescent<ESolutionHFM>(eval, vLS)};
    // vnd->setVerbose();

    // ilsPert = new
    // ILSLPerturbationLPlus2<RepEFP,OPTFRAME_DEFAULT_ADS>(*eval, 50,
    // *nsModifyFuzzyRules, rg); //TODO check why 50 was removed

    // ilsPert = new ILSLPerturbationLPlus2<ESolutionHFM>(*eval,
    // *nsModifyFuzzyRules, rg);
    ilsPert = sptr<ILSLPerturbationLPlus2<ESolutionHFM, EvaluationHFM>>{
        new ILSLPerturbationLPlus2<ESolutionHFM>(eval, nsModifyFuzzyRulesSO,
                                                 rg)};
    // ilsPert->add_ns(*nsChangeSingleInput);
    // nsVAlpha

    // GeneralEvaluator<ESolutionHFM>& geval = *eval;
    // InitialSearch<XES>& constructive

    sref<LocalSearch<ESolutionHFM>> _vnd = vnd;
    sref<ILSLPerturbation<ESolutionHFM>> _ilsPert = ilsPert;
    ils = sptr<IteratedLocalSearchLevels<ESolutionHFM>>{
        new IteratedLocalSearchLevels<ESolutionHFM>(eval, c, _vnd, _ilsPert,
                                                    100, 10)};

    int mu = methodParam.getESMU();
    int lambda = methodParam.getESLambda();
    int esGMaxWithoutImp = methodParam.getESMaxG();
    // int initialDesv = methodParam.getESInitialDesv(); //Parameter
    // for ESContinous - Used in Clemson's Paper int mutationDesv
    // = methodParam.getESMutationDesv(); //Parameter for ESContinous - Used in
    // Clemson's Paper

    // cout<<mu<<"\t"<<lambda<<"\t"<<esMaxG<<"\t"<<initialDesv<<"\t"<<mutationDesv<<endl;
    // getchar();

    // Old continous Evolution Strategy - Deprecated
    // EsCOpt = new EFPESContinous(*eval, *c, vNSeq, emptyLS, mu,
    // lambda, esMaxG, rg, initialDesv, mutationDesv);

    // olr = new OptimalLinearRegression(*eval, *p);
    //
    // vNSeq = new vector<NSSeq<ESolutionHFM>*>;
    vNSeq.push_back(nsModifyFuzzyRulesSO);
    vNSeq.push_back(nsChangeSingleInput);
    vNSeq.push_back(nsRemoveSingleInput);
    vNSeq.push_back(nsAddSingleInput);
    //
    // vNSeqMO = new vector<NSSeq<EMSolutionHFM, MultiEvaluationHFM>*>;
    vNSeqMO.push_back(nsModifyFuzzyRulesMO);
    vNSeqMO.push_back(nsChangeSingleInputMO);
    vNSeqMO.push_back(nsRemoveSingleInputMO);
    vNSeqMO.push_back(nsAddSingleInputMO);

    // vNSeq.push_back(nsVAlpha);
    // vNSeq.push_back(nsAddMean01);
    // vNSeq.push_back(nsAddMean1);
    // vNSeq.push_back(nsAddMeanD15);
    // vNSeq.push_back(nsAddMeanD6);
    // vNSeq.push_back(nsAddMeanD2);
    // vNSeq.push_back(nsAddMean);
    // vNSeq.push_back(nsAddMeanM2);
    // vNSeq.push_back(nsAddMeanM5);
    // vNSeq.push_back(nsAddMeanBigM);

    // TODO check why ES goes more generations some time when we do not have
    // improvements.

    vector<int> vNSeqMax(vNSeq.size(), 1000);
    double mutationRate = 0.1;
    int selectionType = 1;
    vsref<NS<ESolutionHFM>> vNSSeqForNGES;
    vNSSeqForNGES.push_back(nsModifyFuzzyRulesSO);
    vNSSeqForNGES.push_back(nsChangeSingleInput);
    vNSSeqForNGES.push_back(nsRemoveSingleInput);
    vNSSeqForNGES.push_back(nsAddSingleInput);

    string outputFile = "LogPopFOPlus";
    ngesParams = sptr<NGESParams>{
        new NGESParams(vNSeqMax, selectionType, mutationRate, mu, lambda,
                       esGMaxWithoutImp, outputFile, 0)};
    es = sptr<NGES<ESolutionHFM>>{new NGES<ESolutionHFM>(
        eval, c, vNSSeqForNGES, emptyLS, rg, *ngesParams)};
    es->setMessageLevel(LogLevel::Info);

    // MO -- HFM MULTI IS ABLE TO ONLY "EVALUATE" once
    // vector<Evaluator<RepEFP>*> v_e;
    // v_e.push_back(new EFPEvaluator(*p, problemParam, MAPE_INDEX,
    // 0)); v_e.push_back(new EFPEvaluator(*p, problemParam,
    // MAPE_INV_INDEX,
    // 0)); v_e.push_back(new EFPEvaluator(*p, problemParam,
    // RMSE_INDEX, 0)); v_e.push_back(new EFPEvaluator(*p, problemParam,
    // SMAPE_INDEX, 0));
    //// v_e.push_back(new EFPEvaluator(*p, problemParam,
    // / WMAPE_INDEX, 0)); / v_e.push_back(new EFPEvaluator(*p,
    /// problemParam, MMAPE_INDEX, 0));
    // mev = new MultiEvaluator<RepEFP>(v_e);
    //
    // Base to Derived
    sptr<HFMEvaluator> _hfmeval =
        std::static_pointer_cast<HFMEvaluator>(eval.sptr());
    //
    //
    mev = sptr<HFMMultiEvaluator>{new HFMMultiEvaluator(_hfmeval)};
    imev = sptr<IEvaluator<EMSolutionHFM>>{new HFMMultiEvaluator(_hfmeval)};
    //
    sptr<GeneralEvaluator<EMSolutionHFM>> _sptrmev =
        std::static_pointer_cast<GeneralEvaluator<EMSolutionHFM>>(mev);
    //
    sref<GeneralEvaluator<EMSolutionHFM>> gmev = _sptrmev;  // mev;
    //
    moILSPert = sptr<MOILSLPerturbation<EMSolutionHFM, MultiEvaluationHFM>>{
        new MOILSLPerturbationLPlus2<EMSolutionHFM, MultiEvaluationHFM>(
            gmev, nsModifyFuzzyRulesMO, rg)};
    // moILSPert->add_ns(*nsChangeSingleInput);

    basicMOILSPert =
        sptr<BasicMOILSPerturbation<EMSolutionHFM, MultiEvaluationHFM>>{
            new BasicMOILSPerturbation<EMSolutionHFM, MultiEvaluationHFM>(
                gmev, 2, 10, nsModifyFuzzyRulesMO, rg)};
    // basicMOILSPert->add_ns(*nsChangeSingleInput);

    // Trying to checkmodule
    checkModule.add(c);

    sptr<Evaluator<SolutionHFM, EvaluationHFM, ESolutionHFM>> eval1 = _hfmeval;
    sref<Evaluator<SolutionHFM, EvaluationHFM, ESolutionHFM>> eval2 = eval1;
    checkModule.addEvaluator(eval2);  // eval

    checkModule.add(nsModifyFuzzyRulesSO);
    checkModule.add(nsRemoveSingleInput);
    checkModule.add(nsChangeSingleInput);
    checkModule.add(
        nsAddSingleInput);  // This move has dynamic components - Thus
                            // SimpleCost does not work properly

    // checkModule.run(5,2);
    // getchar();
  }

  virtual ~ForecastClass() {
    // tForecastings.clear();

    delete p;
    // delete eval;
    // delete c;

    // for (int i = 0; i < (int)vLS.size(); i++)
    //    delete vLS[i];
    vLS.clear();

    // for (int i = 0; i < (int)vNSeq.size(); i++)
    //    delete vNSeq->at(i);
    // delete vNSeq;
    vNSeq.clear();

    // delete ilsPert; //todo verify
    // delete ils;     //todo verify
    // delete vnd;
    // delete EsCOpt;

    // mev->clear();
    // delete mev;
    // delete es;
    // delete ngesParams;
    // delete basicMOILSPert;
    // delete moILSPert;
  }

  // //add solution to pareto front evaluating with forecasting class
  // evaluators void addSolToParetoWithFCMEV(Solution<RepEFP>& s,
  // Pareto<RepEFP>& pf)
  // {
  // pf.push_back(s, *mev);
  // }

  // add solution to pareto front evaluating with forecasting class evaluators
  void addSolToParetoWithParetoManager(Pareto<EMSolutionHFM>& pf,
                                       const SolutionHFM& candidateS) {
    ParetoManager<ESolutionHFM, EMSolutionHFM> paretoMan(*mev);
    paretoMan.addSolution(pf, candidateS);
  }

  // add solution to pareto front evaluating with forecasting class evaluators
  // void addSolWithMevToParetoWithParetoManager(Pareto<SolutionHFM>& pf, const
  // SolutionHFM& candidateS, const MultiEvaluation<>& candidateMev)
  void addSolWithMevToParetoWithParetoManager(Pareto<EMSolutionHFM>& pf,
                                              const EMSolutionHFM& cand_smev) {
    ParetoManager<ESolutionHFM, EMSolutionHFM> paretoMan(*mev);
    // paretoMan.addSolutionWithMEV(pf, candidateS, candidateMev);
    paretoMan.addSolutionWithMEV(pf, cand_smev);
  }

  // Pareto<SolutionHFM> runMultiObjSearch(double timeGPLS, Pareto<SolutionHFM>*
  // _pf = nullptr)
  void runMultiObjSearch(double timeGPLS, op<Pareto<EMSolutionHFM>> ioPF) {
    // Pareto<SolutionHFM>* pf = new Pareto<SolutionHFM>();

    // if (vS != nullptr)
    // {
    //   if (pf == nullptr)
    //   {
    //     pf = new Pareto<RepEFP>();
    //
    //     for (int ns = 0; ns < vS.size(); ns++)
    //       pf->push_back(*vS[ns], mev);
    //     }
    //     else
    //     {
    //       for (int ns = 0; ns < vS.size(); ns++)
    //         pf->push_back(*vS[ns], mev);
    //     }
    //  }

    int initial_population_size = 30;
    // GRInitialPopulation<RepEFP,OPTFRAME_DEFAULT_ADS> bip(*c, rg, 1);
    // MOVNSLevels<RepEFP> multiobjectvns(v_e, bip,
    // initial_population_size, vNSeq, rg, 10, 10);
    // GRInitialPareto<RepEFP,OPTFRAME_DEFAULT_ADS> grIP(*c, rg, 1, *mev);

    // BasicInitialPareto(InitialSearch<XMES>& _constructive,
    // GeneralEvaluator<XMES>& _mev) :

    ParetoManager<ESolutionHFM, EMSolutionHFM>* paretoMan =
        new ParetoManager<ESolutionHFM, EMSolutionHFM>(*mev);
    // BasicInitialPareto<SolutionHFM, EvaluationHFM, MultiEvaluationHFM,
    // EMSolutionHFM> grIP(*cm, *mev);
    BasicInitialPareto<ESolutionHFM, EMSolutionHFM> grIP(*cm, *paretoMan);

    sptr<GeneralEvaluator<EMSolutionHFM>> _sptrmev =
        std::static_pointer_cast<GeneralEvaluator<EMSolutionHFM>>(mev);
    //
    // GeneralEvaluator<EMSolutionHFM>* gmev = mev;
    sref<GeneralEvaluator<EMSolutionHFM>> gmev = _sptrmev;  // mev;

    int maxTriesRI = 100;
    MORandomImprovement<ESolutionHFM, EMSolutionHFM> moriMFR(
        gmev, vNSeqMO.at(0), maxTriesRI);
    MORandomImprovement<ESolutionHFM, EMSolutionHFM> moriCSI(
        gmev, vNSeqMO.at(1), maxTriesRI);
    MORandomImprovement<ESolutionHFM, EMSolutionHFM> moriRSI(
        gmev, vNSeqMO.at(2), maxTriesRI);
    MORandomImprovement<ESolutionHFM, EMSolutionHFM> moriASI(
        gmev, vNSeqMO.at(3), maxTriesRI);

    vsref<MOLocalSearch<ESolutionHFM, EMSolutionHFM>> vMOLS;
    vMOLS.push_back(moriMFR);
    vMOLS.push_back(moriRSI);
    vMOLS.push_back(moriASI);
    vMOLS.push_back(moriCSI);

    sptr<MultiEvaluator<ESolutionHFM, EMSolutionHFM>> _mev =
        mev;  // std::static_pointer_cast<HFMEvaluator>(eval.sptr());

    /*
    sref<IEvaluator<XMES>> _mev,
                               sref<InitialPareto<XMES>> _init_pareto,
                               int _init_pop_size,
                               vsref<MOLocalSearch<XES, XMES>> _vLS
    */
    // sref<IEvaluator<EMSolutionHFM>> _imev =
    //     std::static_pointer_cast<IEvaluator<EMSolutionHFM>>(mev);
    GeneralParetoLocalSearch<ESolutionHFM, EMSolutionHFM> generalPLS(
        _mev, grIP, initial_population_size, vMOLS);

    BasicMOILS<ESolutionHFM, EMSolutionHFM> basicMOILS(
        _mev, grIP, initial_population_size, moriASI, rg, basicMOILSPert, 100);

    // for testing OptFrame v4
    // BasicGeneralILS<SolutionHFM> basicGeneralILS(*mev, grIP,
    // initial_population_size, &moriASI, rg, *basicMOILSPert, 100);

    int moIlslevelMax = 10;
    int moIlsIterMax = 100;
    MOILSLevels<ESolutionHFM, EMSolutionHFM> moILSLevels(
        _mev, grIP, initial_population_size, moriASI, rg, moILSPert,
        moIlsIterMax, moIlslevelMax);
    // moILSLevels.setMessageLevel(3);

    StopCriteria<MultiEvaluationHFM> moStopCriteriaGPLS;
    moStopCriteriaGPLS.timelimit = timeGPLS;

    /*
      Pareto<SolutionHFM> pfs;
      if (_pf == nullptr) {
         //delete pf;
         pfs = generalPLS.searchWithOptionalPareto(moStopCriteriaGPLS);
         //			pf = moILSLevels.search(moStopCriteriaGPLS);
      } else {
         //delete pf;
         pfs = generalPLS.searchWithOptionalPareto(moStopCriteriaGPLS, _pf);
         //			pf = moILSLevels.search(moStopCriteriaGPLS,
      _pf);
      }
*/
    generalPLS.searchWithOptionalPareto(moStopCriteriaGPLS, ioPF);

    // vector<MultiEvaluation<>*> vEval = pf->getParetoFront();
    // vector<Solution<RepEFP>*> vSolPf = pf->getParetoSet();
    //
    // int nObtainedParetoSol = vEval.size();
    // for (int i = 0; i < nObtainedParetoSol; i++)
    // {
    // //vector<double> solEvaluations;
    // for (int e = 0; e < vEval[i]->size(); e++)
    // cout << vEval[i]->at(e).getObjFunction() <<
    // "\t"; cout << endl;
    //
    // }

    // mev->clear();
    // return pfs;
  }

  std::optional<pair<SolutionHFM, Evaluation<>>> runGRASP(int timeGRASP,
                                                          int nSol) {
    StopCriteria<EvaluationHFM>* stopCriteria =
        new StopCriteria<EvaluationHFM>(timeGRASP);
    std::optional<pair<SolutionHFM, Evaluation<>>> finalSol = std::nullopt;
    delete stopCriteria;
    // BasicGRASP<RepEFP> g(*eval, *c, emptyLS, 0.1, nSol);
    // g.setMessageLevel(3);

    // finalSol = g.search(stopCriteria);
    return finalSol;
  }

  std::optional<ESolutionHFM> run(int timeES, int timeVND, int timeILS) {
    if (timeES == 0) timeES = 1;

    double targetValue = 3.879748973;
    targetValue = 0;

    // StopCriteria<EvaluationHFM> stopCriteria(timeES,
    // Evaluation<>(targetValue));
    StopCriteria<EvaluationHFM> stopCriteria(timeES);
    stopCriteria.target_f = Evaluation<>(targetValue);
    //
    SearchOutput<ESolutionHFM> sout = es->search(stopCriteria);
    std::optional<ESolutionHFM> finalSol = sout.best;  // es->getBestSolution();

    // finalSol = EsCOpt->search(timeES); //Continous ES -- Deprecated

    // vnd->setMessageLevel(3);
    // if (timeVND > 0)
    // vnd->searchFrom(finalSol->first, finalSol->second,
    // timeVND, 0);
    //
    // const Solution<RepEFP> solVND = finalSol->first;
    // const Evaluation<> evaluationVND = finalSol->second;
    // evaluationVND.print();
    // if (timeILS > 0)
    // finalSol = ils->search(timeILS, 0, &solVND,
    // &evaluationVND);

    return finalSol;
  }

  std::optional<ESolutionHFM> runGILS(int timeGRASP, int timeILS) {
    // BasicGRASP<RepEFP> g(*eval, *c, emptyLS, 0.1, 100000);
    // g.setMessageLevel(3);
    //// pair<SolutionHFM, Evaluation<>>* finalSol;

    // stopCriteria.timelimit=timeGRASP;
    // finalSol = g.search(stopCriteria);
    // const Solution<RepEFP> solGRASP = finalSol.first;
    // const Evaluation<> evaluationGrasp = finalSol.second;

    std::optional<ESolutionHFM> finalSol = std::nullopt;
    StopCriteria<EvaluationHFM> stopCriteria;
    stopCriteria.timelimit = timeILS;
    // stopCriteria.target_f(Evaluation<>(0));
    //
    ils->setMessageLevel(LogLevel::Info);
    if (timeILS > 0) {
      // ils->best = finalSol;
      std::cout << "TODO: MUST IMPLEMENT SearchBy to inject parameter "
                   "'finalSol' into ils"
                << std::endl;
      SearchOutput<ESolutionHFM> sout = ils->search(stopCriteria);

      finalSol = *sout.best;  // ils->getBestSolution();
    }
    // finalSol = ils->search(stopCriteria, &solGRASP,
    // &evaluationGrasp);

    return finalSol;
  }

  // Mathematical model for finding optimal weights between models, ensemble
  // forecasting TODO pair<Solution<RepEFP>&, Evaluation<>&>* runOLR()
  // {
  //
  // pair<Solution<RepEFP>&, Evaluation<>&>* finalSol = nullptr;
  // //olr->searchFrom(finalSol->first, 100, 100);
  // return finalSol;
  // }

  // return blind forecasts for the required steps ahead of problemParam class
  // this function feed a trainedModel with the last samples from the vector of
  // TimeSeries
  vector<double>* returnBlind(const RepHFM& trainedModel,
                              vector<vector<double>>& vTimeSeries) {
    return eval->returnForecasts(
        trainedModel, vTimeSeries,
        vTimeSeries[problemParam.getTargetFile()].size(),
        problemParam.getStepsAhead());
  }

  vector<double>* returnErrors(
      const RepHFM& rep, vector<vector<double>>& vForecastingsValidation) {
    return eval->evaluateAll(rep, ALL_EVALUATIONS, &vForecastingsValidation);

    // (NMETRICS, 0);
    // vector<double> estimatedValues = returnForecasts(sol,
    // vForecastingsValidation);
    //
    // int maxLag = problemParam.getMaxLag();
    // vector<double> targetValues;
    //
    // for (int i = maxLag; i < vForecastingsValidation[0].size(); i++)
    // targetValues.push_back(vForecastingsValidation[0][i]);
    //
    //// cout << validationSetValues << endl;
    //// getchar();
    // foIndicatorNew = returnErrorsCallGetAccuracy(targetValues,
    // estimatedValues);
    //// cout << "insideForecastClassNew" << endl;
    //// cout << foIndicatorNew << endl;

    // return foIndicatorNew;
  }

  // return all possible forecasting measures
  vector<double>* callEvalGetAccuracy(vector<double> targetValues,
                                      vector<double> estimatedValues) {
    return eval->getAccuracy(targetValues, estimatedValues, ALL_EVALUATIONS);
  }
  //

  // Return forecasts with pre-defined sliding window strategy with FH
  vector<double>* returnForecasts(
      pair<SolutionHFM, Evaluation<>>& sol,
      vector<vector<double>> vForecastingsValidation) {
    pair<vector<double>*, vector<double>*>* targetAndForecasts =
        eval->generateSWMultiRoundForecasts(sol.first.getR(),
                                            vForecastingsValidation,
                                            problemParam.getStepsAhead());
    return targetAndForecasts->second;
  }

  /*Target and forecasts
         Return forecasts with pre-defined sliding window strategy with FH*/
  pair<vector<double>*, vector<double>*>* returnForecastsAndTargets(
      const RepHFM& rep, const vector<vector<double>> vForecastingsValidation) {
    return eval->generateSWMultiRoundForecasts(rep, vForecastingsValidation,
                                               problemParam.getStepsAhead());
  }

  vector<double>* returnErrorsPersistance(vector<double> targetValues, int fH) {
    vector<double> estimatedValues;
    int nSamples = targetValues.size();
    for (int s = 1; s < nSamples; s += fH)
      for (int k = 0; k < fH; k++) {
        if (s + k < nSamples) estimatedValues.push_back(targetValues[s - 1]);
      }

    // cout<<targetValues<<endl;
    targetValues.erase(targetValues.begin());
    // cout << estimatedValues << endl;
    // cout << targetValues << endl;
    // getchar();

    return eval->getAccuracy(targetValues, estimatedValues, -1);
  }

  void exportForecasts(const vector<double> forecasts, string output) {
    FILE* fResults = fopen(output.c_str(), "w");
    for (int n = 0; n < (int)forecasts.size(); n++) {
      // cout<< forecasts[n]<<endl;
      // getchar();
      fprintf(fResults, "%.10f\n", forecasts[n]);
    }
    fclose(fResults);
  }
};
}  // namespace HFM
#endif  // EXAMPLES_HFM_INCLUDE_HFM_FORECASTCLASS_HPP_
