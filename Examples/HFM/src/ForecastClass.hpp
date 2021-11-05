/*
 * ForecastClass.hpp
 *
 *  Created on: 10/09/2014
 *      Author: vitor
 */

#ifndef HFM_FORECASTCLASS_HPP_
#define HFM_FORECASTCLASS_HPP_

#include "./NS/NSSeqHFMAddSingleInput.hpp"
#include "./NS/NSSeqHFMChangeSingleInput.hpp"
#include "./NS/NSSeqHFMModifyRules.hpp"
#include "./NS/NSSeqHFMRemoveSingleInput.hpp"
#include "./NS/NSSeqHFMVAlpha.hpp"
#include "Evaluation.h"
#include "Evaluator.hpp"
#include "HFMESContinous.hpp"
#include "HFMParams.hpp"
#include "MultiEvaluatorHFM.hpp"
#include "ProblemInstance.hpp"

#include "TreatForecasts.hpp"
#include <OptFrame/Heuristics/Empty.hpp>
#include <OptFrame/Heuristics/EvolutionaryAlgorithms/NGES.hpp>
#include <OptFrame/Heuristics/GRASP/BasicGRASP.hpp>
#include <OptFrame/Heuristics/ILS/ILSLPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/ILS/MO/BasicMOILS.hpp>
#include <OptFrame/Heuristics/ILS/MO/MOILSLPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/MO/MOILSLevels.hpp>
#include <OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/RandomDescentMethod.hpp>
#include <OptFrame/Heuristics/LocalSearches/VND.h>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/Heuristics/MOLocalSearches/GPLS.hpp>
#include <OptFrame/Heuristics/MOLocalSearches/MOBestImprovement.hpp>
#include <OptFrame/Heuristics/MOLocalSearches/MORandomImprovement.hpp>
#include <OptFrame/MultiEvaluator.hpp>
#include <OptFrame/MultiObjSearch.hpp>
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/Util/CheckCommand.hpp>

namespace HFM {

class ForecastClass
{
private:
   vector<vector<double>>& tForecastings;
   ProblemParameters& problemParam;
   RandGen& rg;
   HFMParams& methodParam;

   HFMProblemInstance* p;
   //HFMEvaluator* eval;
   sref<GeneralEvaluator<ESolutionHFMVRP>> eval;

   //Constructive<SolutionHFM>* c;
   InitialSearch<ESolutionHFM, EvaluationHFM>* c;        // for single obj
   InitialSearch<EMSolutionHFM, MultiEvaluationHFM>* cm; // for multi obj
   //
   vector<NSSeq<ESolutionHFM>*> vNS;

   EmptyLocalSearch<ESolutionHFM> emptyLS;
   vector<NSSeq<ESolutionHFM>*>* vNSeq;
   vector<NSSeq<EMSolutionHFM, MultiEvaluationHFM>*>* vNSeqMO;

   //	EFPESContinous* EsCOpt;
   NGES<SolutionHFM>* es;
   NGESParams* ngesParams;

   vector<LocalSearch<ESolutionHFM>*> vLS;

   VariableNeighborhoodDescent<ESolutionHFM>* vnd;
   IteratedLocalSearchLevels<ESolutionHFM>* ils;

   vector<vector<double>> vBlindResults;
   vector<pair<SolutionHFM, Evaluation<>>*> vFinalSol;

   ILSLPerturbationLPlus2<ESolutionHFM, EvaluationHFM>* ilsPert;
   //
   // MO versions
   //MOILSLPerturbationLPlus2<SolutionHFM>* moILSPert;
   MOILSLPerturbationLPlus2<EMSolutionHFM, MultiEvaluationHFM>* moILSPert;
   //BasicMOILSPerturbation<SolutionHFM>* basicMOILSPert;
   BasicMOILSPerturbation<EMSolutionHFM, MultiEvaluationHFM>* basicMOILSPert;

   //OptimalLinearRegression* olr;
   //	MultiEvaluator<RepEFP>* mev;
   HFMMultiEvaluator* mev;
   CheckCommand<RepHFM, OPTFRAME_DEFAULT_ADS, SolutionHFM> checkModule;

public:
   ForecastClass(vector<vector<double>>& _tForecastings, ProblemParameters& _problemParam, RandGen& _rg, HFMParams& _methodParam)
     : tForecastings(_tForecastings)
     , problemParam(_problemParam)
     , rg(_rg)
     , methodParam(_methodParam)
   {

      p = new HFMProblemInstance(tForecastings);
      eval = new HFMEvaluator(*p, problemParam, methodParam.getEvalFOMinimizer(), methodParam.getEvalAprox());

      //NSSeqHFMModifyRules* nsModifyFuzzyRules = new NSSeqHFMModifyRules(*p, rg);
      auto nsModifyFuzzyRulesSO = new NSSeqHFMModifyRules<>(*p, rg);
      auto nsModifyFuzzyRulesMO = new NSSeqHFMModifyRules<EMSolutionHFM, MultiEvaluationHFM>(*p, rg);

      auto nsChangeSingleInput = new NSSeqHFMChangeSingleInput<>(*p, rg, problemParam.getVMaxLag(), problemParam.getVMaxUpperLag());
      auto nsChangeSingleInputMO = new NSSeqHFMChangeSingleInput<EMSolutionHFM, MultiEvaluationHFM>(*p, rg, problemParam.getVMaxLag(), problemParam.getVMaxUpperLag());

      auto nsRemoveSingleInput = new NSSeqHFMRemoveSingleInput<>(rg);
      auto nsRemoveSingleInputMO = new NSSeqHFMRemoveSingleInput<EMSolutionHFM, MultiEvaluationHFM>(rg);

      auto nsAddSingleInput = new NSSeqNEIGHAddSingleInput<>(*p, rg, problemParam.getVMaxLag(), problemParam.getVMaxUpperLag());
      auto nsAddSingleInputMO = new NSSeqNEIGHAddSingleInput<EMSolutionHFM, MultiEvaluationHFM>(*p, rg, problemParam.getVMaxLag(), problemParam.getVMaxUpperLag());
      //
      //		NSSeqNEIGHVAlpha* nsVAlpha = new NSSeqNEIGHVAlpha(*p, rg, 5);

      //		NSSeqNEIGHAddX* nsAddMean01 = new NSSeqNEIGHAddX(*p, rg, 0.1);
      //		NSSeqNEIGHAddX* nsAddMean1 = new NSSeqNEIGHAddX(*p, rg, 1);
      //		NSSeqNEIGHAddX* nsAddMeanD15 = new NSSeqNEIGHAddX(*p, rg, p->getMean(0) / 15);
      //		NSSeqNEIGHAddX* nsAddMeanD6 = new NSSeqNEIGHAddX(*p, rg, p->getMean(0) / 6);
      //		NSSeqNEIGHAddX* nsAddMeanD2 = new NSSeqNEIGHAddX(*p, rg, p->getMean(0) / 2);
      //		NSSeqNEIGHAddX* nsAddMean = new NSSeqNEIGHAddX(*p, rg, p->getMean(0));
      //		NSSeqNEIGHAddX* nsAddMeanM2 = new NSSeqNEIGHAddX(*p, rg, 2 * p->getMean(0));
      //		NSSeqNEIGHAddX* nsAddMeanM5 = new NSSeqNEIGHAddX(*p, rg, 5 * p->getMean(0));
      //		NSSeqNEIGHAddX* nsAddMeanBigM = new NSSeqNEIGHAddX(*p, rg, 100 * p->getMean(0));

      int cMethod = methodParam.getConstrutiveMethod();
      int cPre = methodParam.getConstrutivePrecision();

      if (cMethod == 0) {
         c = new ConstructiveRandom<ESolutionHFM, EvaluationHFM>(*p, problemParam, rg, cPre);
         cm = new ConstructiveRandom<EMSolutionHFM, MultiEvaluationHFM>(*p, problemParam, rg, cPre);
      }
      if (cMethod == 2) {
         c = new ConstructiveACF<ESolutionHFM, EvaluationHFM>(*p, problemParam, rg, cPre, methodParam.getConstrutiveLimitAlphaACF());
         cm = new ConstructiveACF<EMSolutionHFM, MultiEvaluationHFM>(*p, problemParam, rg, cPre, methodParam.getConstrutiveLimitAlphaACF());
      }

      //FirstImprovement<SolutionHFM>* fiModifyFuzzyRules = new FirstImprovement<SolutionHFM>(*eval, *nsModifyFuzzyRules);
      //FirstImprovement<SolutionHFM>* fiChangeSingleInput = new FirstImprovement<SolutionHFM>(*eval, *nsChangeSingleInput);
      RandomDescentMethod<ESolutionHFM>* rdmRemove = new RandomDescentMethod<ESolutionHFM>(*eval, *nsRemoveSingleInput, 500); //		FirstImprovement<RepEFP>* fiVAlpha = new FirstImprovement<RepEFP>(*eval, *nsVAlpha);
      RandomDescentMethod<ESolutionHFM>* rdmAdd = new RandomDescentMethod<ESolutionHFM>(*eval, *nsAddSingleInput, 500);       //		FirstImprovement<RepEFP>* fiVAlpha = new FirstImprovement<RepEFP>(*eval, *nsVAlpha);
                                                                                                                              //		int maxRDM = 100;
                                                                                                                              //
                                                                                                                              //		//rdm->setMessageLevel(3);
                                                                                                                              //		vLS.push_back(fiVAlpha);
      vLS.push_back(rdmRemove);
      vLS.push_back(rdmAdd);
      //		vLS.push_back(fiModifyFuzzyRules);
      //		vLS.push_back(fiChangeSingleInput);
      vnd = new VariableNeighborhoodDescent<ESolutionHFM>(*eval, vLS);
      //		vnd->setVerbose();

      //		ilsPert = new ILSLPerturbationLPlus2<RepEFP,OPTFRAME_DEFAULT_ADS>(*eval, 50, *nsModifyFuzzyRules, rg); //TODO check why 50 was removed

      //ilsPert = new ILSLPerturbationLPlus2<ESolutionHFM>(*eval, *nsModifyFuzzyRules, rg);
      ilsPert = new ILSLPerturbationLPlus2<ESolutionHFM>(*eval, *nsModifyFuzzyRulesSO, rg);
      //		ilsPert->add_ns(*nsChangeSingleInput);
      //		nsVAlpha

      GeneralEvaluator<ESolutionHFM>& geval = *eval;
      //InitialSearch<XES, XEv>& constructive

      ils = new IteratedLocalSearchLevels<ESolutionHFM>(geval, *c, *vnd, *ilsPert, 100, 10);

      int mu = methodParam.getESMU();
      int lambda = methodParam.getESLambda();
      int esGMaxWithoutImp = methodParam.getESMaxG();
      //		int initialDesv = methodParam.getESInitialDesv(); //Parameter for ESContinous - Used in Clemson's Paper
      //		int mutationDesv = methodParam.getESMutationDesv(); //Parameter for ESContinous - Used in Clemson's Paper

      //cout<<mu<<"\t"<<lambda<<"\t"<<esMaxG<<"\t"<<initialDesv<<"\t"<<mutationDesv<<endl;
      //getchar();

      //Old continous Evolution Strategy - Deprecated
      //		EsCOpt = new EFPESContinous(*eval, *c, vNSeq, emptyLS, mu, lambda, esMaxG, rg, initialDesv, mutationDesv);

      //olr = new OptimalLinearRegression(*eval, *p);
      vNSeq = new vector<NSSeq<ESolutionHFM>*>;
      vNSeq->push_back(nsModifyFuzzyRulesSO);
      vNSeq->push_back(nsChangeSingleInput);
      vNSeq->push_back(nsRemoveSingleInput);
      vNSeq->push_back(nsAddSingleInput);
      //
      vNSeqMO = new vector<NSSeq<EMSolutionHFM, MultiEvaluationHFM>*>;
      vNSeqMO->push_back(nsModifyFuzzyRulesMO);
      vNSeqMO->push_back(nsChangeSingleInputMO);
      vNSeqMO->push_back(nsRemoveSingleInputMO);
      vNSeqMO->push_back(nsAddSingleInputMO);

      //		vNSeq.push_back(nsVAlpha);
      //		vNSeq.push_back(nsAddMean01);
      //		vNSeq.push_back(nsAddMean1);
      //		vNSeq.push_back(nsAddMeanD15);
      //		vNSeq.push_back(nsAddMeanD6);
      //		vNSeq.push_back(nsAddMeanD2);
      //		vNSeq.push_back(nsAddMean);
      //		vNSeq.push_back(nsAddMeanM2);
      //		vNSeq.push_back(nsAddMeanM5);
      //		vNSeq.push_back(nsAddMeanBigM);

      //TODO check why ES goes more generations some time when we do not have improvements.

      vector<int> vNSeqMax(vNSeq->size(), 1000);
      double mutationRate = 0.1;
      int selectionType = 1;
      vector<NS<ESolutionHFM>*> vNSSeqForNGES;
      vNSSeqForNGES.push_back(nsModifyFuzzyRulesSO);
      vNSSeqForNGES.push_back(nsChangeSingleInput);
      vNSSeqForNGES.push_back(nsRemoveSingleInput);
      vNSSeqForNGES.push_back(nsAddSingleInput);

      string outputFile = "LogPopFOPlus";
      ngesParams = new NGESParams(vNSeqMax, selectionType, mutationRate, mu, lambda, esGMaxWithoutImp, outputFile, 0);
      es = new NGES<SolutionHFM>(*eval, *c, vNSSeqForNGES, emptyLS, rg, *ngesParams);
      es->setMessageLevel(3);

      //MO -- HFM MULTI IS ABLE TO ONLY "EVALUATE" once
      //		vector<Evaluator<RepEFP>*> v_e;
      //		v_e.push_back(new EFPEvaluator(*p, problemParam, MAPE_INDEX, 0));
      //		v_e.push_back(new EFPEvaluator(*p, problemParam, MAPE_INV_INDEX, 0));
      //		v_e.push_back(new EFPEvaluator(*p, problemParam, RMSE_INDEX, 0));
      //		v_e.push_back(new EFPEvaluator(*p, problemParam, SMAPE_INDEX, 0));
      ////		v_e.push_back(new EFPEvaluator(*p, problemParam, WMAPE_INDEX, 0));
      ////		v_e.push_back(new EFPEvaluator(*p, problemParam, MMAPE_INDEX, 0));
      //		mev = new MultiEvaluator<RepEFP>(v_e);
      mev = new HFMMultiEvaluator(*eval);
      GeneralEvaluator<EMSolutionHFM, MultiEvaluationHFM>* gmev = mev;
      moILSPert = new MOILSLPerturbationLPlus2<EMSolutionHFM, MultiEvaluationHFM>(*gmev, *nsModifyFuzzyRulesMO, rg);
      //		moILSPert->add_ns(*nsChangeSingleInput);

      basicMOILSPert = new BasicMOILSPerturbation<EMSolutionHFM, MultiEvaluationHFM>(*gmev, 2, 10, *nsModifyFuzzyRulesMO, rg);
      //		basicMOILSPert->add_ns(*nsChangeSingleInput);

      //Trying to checkmodule
      checkModule.add(*c);
      checkModule.add(*eval);

      checkModule.add(*nsModifyFuzzyRulesSO);
      checkModule.add(*nsRemoveSingleInput);
      checkModule.add(*nsChangeSingleInput);
      checkModule.add(*nsAddSingleInput); //This move has dynamic components - Thus SimpleCost does not work properly

      //		checkModule.run(5,2);
      //		getchar();
   }

   virtual ~ForecastClass()
   {
      //		tForecastings.clear();

      delete p;
      delete eval;
      delete c;

      for (int i = 0; i < (int)vLS.size(); i++)
         delete vLS[i];
      vLS.clear();

      for (int i = 0; i < (int)vNSeq->size(); i++)
         delete vNSeq->at(i);
      delete vNSeq;

      delete ilsPert; //todo verify
      delete ils;     //todo verify
      delete vnd;
      //		delete EsCOpt;

      //		mev->clear();
      delete mev;
      delete es;
      delete ngesParams;
      delete basicMOILSPert;
      delete moILSPert;
   }

   //	//add solution to pareto front evaluating with forecasting class evaluators
   //	void addSolToParetoWithFCMEV(Solution<RepEFP>& s, Pareto<RepEFP>& pf)
   //	{
   //		pf.push_back(s, *mev);
   //	}

   //add solution to pareto front evaluating with forecasting class evaluators
   void addSolToParetoWithParetoManager(Pareto<SolutionHFM>& pf, const SolutionHFM& candidateS)
   {
      paretoManager<SolutionHFM> paretoMan(*mev);
      paretoMan.addSolution(pf, candidateS);
   }

   //add solution to pareto front evaluating with forecasting class evaluators
   //void addSolWithMevToParetoWithParetoManager(Pareto<SolutionHFM>& pf, const SolutionHFM& candidateS, const MultiEvaluation<>& candidateMev)
   void addSolWithMevToParetoWithParetoManager(Pareto<SolutionHFM>& pf, const EMSolutionHFM& cand_smev)
   {
      paretoManager<SolutionHFM> paretoMan(*mev);
      //paretoMan.addSolutionWithMEV(pf, candidateS, candidateMev);
      paretoMan.addSolutionWithMEV(pf, cand_smev);
   }

   //Pareto<SolutionHFM> runMultiObjSearch(double timeGPLS, Pareto<SolutionHFM>* _pf = nullptr)
   void runMultiObjSearch(double timeGPLS, op<Pareto<SolutionHFM>> ioPF)
   {
      //Pareto<SolutionHFM>* pf = new Pareto<SolutionHFM>();

      //		if (vS != nullptr)
      //		{
      //			if (pf == nullptr)
      //			{
      //				pf = new Pareto<RepEFP>();
      //
      //				for (int ns = 0; ns < vS.size(); ns++)
      //					pf->push_back(*vS[ns], mev);
      //			}
      //			else
      //			{
      //				for (int ns = 0; ns < vS.size(); ns++)
      //					pf->push_back(*vS[ns], mev);
      //			}
      //		}

      int initial_population_size = 30;
      //		GRInitialPopulation<RepEFP,OPTFRAME_DEFAULT_ADS> bip(*c, rg, 1);
      //		MOVNSLevels<RepEFP> multiobjectvns(v_e, bip, initial_population_size, vNSeq, rg, 10, 10);
      //		GRInitialPareto<RepEFP,OPTFRAME_DEFAULT_ADS> grIP(*c, rg, 1, *mev);

      //BasicInitialPareto(InitialSearch<XMES, XMEv>& _constructive, GeneralEvaluator<XMES, XMEv>& _mev) :

      paretoManager<SolutionHFM>* paretoMan = new paretoManager<SolutionHFM>(*mev);
      //BasicInitialPareto<SolutionHFM, EvaluationHFM, MultiEvaluationHFM, EMSolutionHFM> grIP(*cm, *mev);
      BasicInitialPareto<SolutionHFM, MultiEvaluationHFM> grIP(*cm, *paretoMan);

      GeneralEvaluator<EMSolutionHFM, MultiEvaluationHFM>* gmev = mev;

      int maxTriesRI = 100;
      MORandomImprovement<SolutionHFM> moriMFR(*gmev, *vNSeqMO->at(0), maxTriesRI);
      MORandomImprovement<SolutionHFM> moriCSI(*gmev, *vNSeqMO->at(1), maxTriesRI);
      MORandomImprovement<SolutionHFM> moriRSI(*gmev, *vNSeqMO->at(2), maxTriesRI);
      MORandomImprovement<SolutionHFM> moriASI(*gmev, *vNSeqMO->at(3), maxTriesRI);

      vector<MOLocalSearch<SolutionHFM>*> vMOLS;
      vMOLS.push_back(&moriMFR);
      vMOLS.push_back(&moriRSI);
      vMOLS.push_back(&moriASI);
      vMOLS.push_back(&moriCSI);

      GeneralParetoLocalSearch<SolutionHFM> generalPLS(*mev, grIP, initial_population_size, vMOLS);

      BasicMOILS<SolutionHFM, MultiEvaluationHFM> basicMOILS(*mev, grIP, initial_population_size, &moriASI, rg, *basicMOILSPert, 100);

      // for testing OptFrame v4
      //BasicGeneralILS<SolutionHFM> basicGeneralILS(*mev, grIP, initial_population_size, &moriASI, rg, *basicMOILSPert, 100);

      int moIlslevelMax = 10;
      int moIlsIterMax = 100;
      MOILSLevels<SolutionHFM> moILSLevels(*mev, grIP, initial_population_size, &moriASI, rg, *moILSPert, moIlsIterMax, moIlslevelMax);
      //		moILSLevels.setMessageLevel(3);

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
         //			pf = moILSLevels.search(moStopCriteriaGPLS, _pf);
      }
*/
      generalPLS.searchWithOptionalPareto(moStopCriteriaGPLS, ioPF);

      //		vector<MultiEvaluation<>*> vEval = pf->getParetoFront();
      //		vector<Solution<RepEFP>*> vSolPf = pf->getParetoSet();
      //
      //		int nObtainedParetoSol = vEval.size();
      //		for (int i = 0; i < nObtainedParetoSol; i++)
      //		{
      //			//vector<double> solEvaluations;
      //			for (int e = 0; e < vEval[i]->size(); e++)
      //				cout << vEval[i]->at(e).getObjFunction() << "\t";
      //			cout << endl;
      //
      //		}

      //		mev->clear();
      //return pfs;
   }

   std::optional<pair<SolutionHFM, Evaluation<>>> runGRASP(int timeGRASP, int nSol)
   {
      StopCriteria<EvaluationHFM>* stopCriteria = new StopCriteria<EvaluationHFM>(timeGRASP);
      std::optional<pair<SolutionHFM, Evaluation<>>> finalSol = std::nullopt;
      delete stopCriteria;
      //		BasicGRASP<RepEFP> g(*eval, *c, emptyLS, 0.1, nSol);
      //		g.setMessageLevel(3);

      //		finalSol = g.search(stopCriteria);
      return finalSol;
   }

   std::optional<ESolutionHFM> run(int timeES, int timeVND, int timeILS)
   {
      if (timeES == 0)
         timeES = 1;

      double targetValue = 3.879748973;
      targetValue = 0;

      //StopCriteria<EvaluationHFM> stopCriteria(timeES, Evaluation<>(targetValue));
      StopCriteria<EvaluationHFM> stopCriteria(timeES);
      stopCriteria.target_f = Evaluation<>(targetValue);
      //
      es->search(stopCriteria);
      std::optional<ESolutionHFM> finalSol = es->getBestSolution();

      //finalSol = EsCOpt->search(timeES); //Continous ES -- Deprecated

      //		vnd->setMessageLevel(3);
      //		if (timeVND > 0)
      //			vnd->searchFrom(finalSol->first, finalSol->second, timeVND, 0);
      //
      //		const Solution<RepEFP> solVND = finalSol->first;
      //		const Evaluation<> evaluationVND = finalSol->second;
      //		evaluationVND.print();
      //		if (timeILS > 0)
      //			finalSol = ils->search(timeILS, 0, &solVND, &evaluationVND);

      return finalSol;
   }

   std::optional<ESolutionHFM> runGILS(int timeGRASP, int timeILS)
   {

      //		BasicGRASP<RepEFP> g(*eval, *c, emptyLS, 0.1, 100000);
      //		g.setMessageLevel(3);
      ////pair<SolutionHFM, Evaluation<>>* finalSol;

      //		stopCriteria.timelimit=timeGRASP;
      //		finalSol = g.search(stopCriteria);
      //		const Solution<RepEFP> solGRASP = finalSol.first;
      //		const Evaluation<> evaluationGrasp = finalSol.second;

      std::optional<ESolutionHFM> finalSol = std::nullopt;
      StopCriteria<EvaluationHFM> stopCriteria;
      stopCriteria.timelimit = timeILS;
      //stopCriteria.target_f(Evaluation<>(0));
      //
      ils->setMessageLevel(3);
      if (timeILS > 0) {
         ils->best = finalSol;
         ils->search(stopCriteria);
         finalSol = ils->getBestSolution();
      }
      //		finalSol = ils->search(stopCriteria, &solGRASP, &evaluationGrasp);

      return finalSol;
   }

   //Mathematical model for finding optimal weights between models, ensemble forecasting TODO
   //	pair<Solution<RepEFP>&, Evaluation<>&>* runOLR()
   //	{
   //
   //		pair<Solution<RepEFP>&, Evaluation<>&>* finalSol = nullptr;
   //		//olr->searchFrom(finalSol->first, 100, 100);
   //		return finalSol;
   //	}

   //return blind forecasts for the required steps ahead of problemParam class
   //this function feed a trainedModel with the last samples from the vector of TimeSeries
   vector<double>* returnBlind(const RepHFM& trainedModel, vector<vector<double>>& vTimeSeries)
   {
      return eval->returnForecasts(trainedModel, vTimeSeries, vTimeSeries[problemParam.getTargetFile()].size(), problemParam.getStepsAhead());
   }

   vector<double>* returnErrors(const RepHFM& rep, vector<vector<double>>& vForecastingsValidation)
   {
      return eval->evaluateAll(rep, ALL_EVALUATIONS, &vForecastingsValidation);

      //		(NMETRICS, 0);
      //		vector<double> estimatedValues = returnForecasts(sol, vForecastingsValidation);
      //
      //		int maxLag = problemParam.getMaxLag();
      //		vector<double> targetValues;
      //
      //		for (int i = maxLag; i < vForecastingsValidation[0].size(); i++)
      //			targetValues.push_back(vForecastingsValidation[0][i]);
      //
      ////		cout << validationSetValues << endl;
      ////		getchar();
      //		foIndicatorNew = returnErrorsCallGetAccuracy(targetValues, estimatedValues);
      ////		cout << "insideForecastClassNew" << endl;
      ////		cout << foIndicatorNew << endl;

      //		return foIndicatorNew;
   }

   //return all possible forecasting measures
   vector<double>* callEvalGetAccuracy(vector<double> targetValues, vector<double> estimatedValues)
   {
      return eval->getAccuracy(targetValues, estimatedValues, ALL_EVALUATIONS);
   }
   //

   //Return forecasts with pre-defined sliding window strategy with FH
   vector<double>* returnForecasts(pair<SolutionHFM, Evaluation<>>& sol, vector<vector<double>> vForecastingsValidation)
   {
      pair<vector<double>*, vector<double>*>* targetAndForecasts = eval->generateSWMultiRoundForecasts(sol.first.getR(), vForecastingsValidation, problemParam.getStepsAhead());
      return targetAndForecasts->second;
   }

   /*Target and forecasts
	 Return forecasts with pre-defined sliding window strategy with FH*/
   pair<vector<double>*, vector<double>*>* returnForecastsAndTargets(const RepHFM& rep, const vector<vector<double>> vForecastingsValidation)
   {
      return eval->generateSWMultiRoundForecasts(rep, vForecastingsValidation, problemParam.getStepsAhead());
   }

   vector<double>* returnErrorsPersistance(vector<double> targetValues, int fH)
   {

      vector<double> estimatedValues;
      int nSamples = targetValues.size();
      for (int s = 1; s < nSamples; s += fH)
         for (int k = 0; k < fH; k++) {
            if (s + k < nSamples)
               estimatedValues.push_back(targetValues[s - 1]);
         }

      //		cout<<targetValues<<endl;
      targetValues.erase(targetValues.begin());
      //		cout << estimatedValues << endl;
      //		cout << targetValues << endl;
      //		getchar();

      return eval->getAccuracy(targetValues, estimatedValues, -1);
   }

   void exportForecasts(const vector<double> forecasts, string output)
   {
      FILE* fResults = fopen(output.c_str(), "w");
      for (int n = 0; n < (int)forecasts.size(); n++) {
         //			cout<< forecasts[n]<<endl;
         //			getchar();
         fprintf(fResults, "%.10f\n", forecasts[n]);
      }
      fclose(fResults);
   }
};
}
#endif /* HFM_FORECASTCLASS_HPP_ */
