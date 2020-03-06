// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_CHECK_COMMAND_HPP_
#define OPTFRAME_CHECK_COMMAND_HPP_

#include "../Timer.hpp"
#include "Matrix.hpp"
#include "KahanSummation.hpp"
#include "../OptFrameList.hpp"

#include "../Evaluator.hpp"
#include "../Constructive.hpp"
#include "../NS.hpp"
#include "../NSSeq.hpp"
#include "../NSEnum.hpp"


namespace optframe
{

//CheckCommand uses SRand seed TODO
//template<XRepresentation R, class ADS, XSolution S = CopySolution<R, ADS>, XEvaluation XEv = Evaluation<>>
// manually passing 'S' (for safety)

// XSH is currently pair<S,XEv> ... will we test both SingleObj and MultiObj here? If that's the case, we will need XSH1 and XSH2 (two search spaces)... better having two checkmodules than this!
template<XRepresentation R, class ADS, XBaseSolution<R, ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>, XSearch<XES> XSH = std::pair<S, XEv>>
class CheckCommand
{

	static const int CMERR_EV_BETTERTHAN = 2001;
	static const int CMERR_EV_BETTEREQUALS = 2002;
	static const int CMERR_MOVE_EQUALS = 3001;
	static const int CMERR_MOVE_HASREVERSE = 3002;
	static const int CMERR_MOVE_REVREV_VALUE = 3004;
	static const int CMERR_MOVE_REVSIMPLE = 3005;
	static const int CMERR_MOVE_REVFASTER = 3006;
	static const int CMERR_MOVE_REALREVFASTER = 3008;
	static const int CMERR_MOVE_COST = 3007;

private:
	bool verbose;

	bool convertNS;

	// independent moves
	bool checkIndependent;

   //vector<GeneralEvaluator<XES>*> lEvaluator;
	vector<Evaluator<S, XEv, XES>*> lEvaluator;
   //vector<GeneralEvaluator<XES>*> lGenEvaluator;
	//vector<Constructive<S>*> lConstructive;
   vector<InitialSearch<XES>*> lConstructive;
	vector<CopySolution<R,ADS>*> lSolution;
	vector<shared_ptr<Move<XES>>> lMove;
	vector<NS<XES, XEv>*> lNS;
	vector<NSSeq<XES>*> lNSSeq;
	vector<NSEnum<XES>*> lNSEnum;

	ADSManager<R, ADS, S>* adsMan;

public:

	vector<ADSManager<R, ADS, S>*> lADSManagerComp; // optional

	CheckCommand(bool _verbose = false) :
			verbose(_verbose)
	{
		convertNS = true;
		adsMan = nullptr;
		checkIndependent = true;
	}

	virtual ~CheckCommand()
	{
	}

	//void add(Constructive<S>& c)
   void add(InitialSearch<XES>& c)
	{
		lConstructive.push_back(&c);
		if (verbose)
			cout << "checkcommand: Constructive " << lConstructive.size() << " added!" << endl;
	}

	void add(ADSManager<R, ADS, S>& adsMan)
	{
		lADSManagerComp.push_back(&adsMan);
		if (verbose)
			cout << "checkcommand: AdsMan " << lADSManagerComp.size() << " added!" << endl;
	}

	void add(Evaluator<S, XEv, XES>& c)
	{
		lEvaluator.push_back(&c);
		if (verbose)
			cout << "checkcommand: Evaluator " << lEvaluator.size() << " added!" << endl;
	}

/*
   void add(GeneralEvaluator<XES>& c)
	{
		lEvaluator.push_back(&c);
		if (verbose)
			cout << "checkcommand: General Evaluator " << lEvaluator.size() << " added!" << endl;
	}
*/
	void add(CopySolution<R,ADS>& c)
	{
		lSolution.push_back(&c);
		if (verbose)
			cout << "checkcommand: Solution " << lSolution.size() << " added!" << endl;
	}

	void add(Move<XES, XEv>& c)
	{
		lMove.push_back(&c);
		if (verbose)
			cout << "checkcommand: Move " << lMove.size() << " added!" << endl;
	}

	void add(NS<XES, XEv>& c)
	{
		lNS.push_back(&c);
		if (verbose)
			cout << "checkcommand: NS " << lNS.size() << " added!" << endl;
	}

	void add(NSSeq<XES, XEv, XSH>& c)
	{
		lNSSeq.push_back(&c);
		if (verbose)
			cout << "checkcommand: NSSeq " << lNSSeq.size() << " added!" << endl;
		if (convertNS)
			add((NS<XES, XEv>&) c);
	}

	void add(NSEnum<XES, XEv>& c)
	{
		lNSEnum.push_back(&c);
		if (verbose)
			cout << "checkcommand: NSEnum " << lNSEnum.size() << " added!" << endl;
		if (convertNS)
			add((NSSeq<XES, XEv>&) c);
	}

	void message(Component* c, int iter, string text)
	{
		message(c->id(), iter, text);
	}

	void message(string component, int iter, string text)
	{
		if (verbose)
			cout << "checkcommand iter: " << iter << " testing component '" << component << "' => " << text << endl;
	}

	void error(string text)
	{
		cout << "checkcommand error: " << text << endl;
	}

	void errormsg(string component, int code, string scode, int iter, string text)
	{
		cout << "checkcommand ERROR " << code << " (" << scode << "): iter=" << iter << " testing component '" << component << "' MSG: " << text << endl;
	}

	bool parseBool(string b)
	{
		return b == "true";
	}

	struct TimeCheckWithSamples
	{
		vector<vector<double> > timeNSApply;
		vector<vector<double> > timeNSCostApply;
		vector<vector<double> > timeNSCostApplyDelta;
		vector<vector<double> > timeNSCostApplyRealDelta;
		vector<vector<double> > timeNSCost;
		vector<vector<double> > timeNSEstimatedCost;
		vector<vector<double> > errorNSEstimatedCost;
		vector<double> timeCloneSolution;
		vector<vector<double> > timeInitializeADS;
		vector<vector<double> > fullTimeEval;
		vector<vector<double> > timeReeval;
		vector<vector<double> > timeConstructive;

		bool overestimate, underestimate;
	};

	bool testMoveGeneral(int iter, NS<XES, XEv>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckWithSamples& timeSamples)
	{
		for (unsigned ev = 0; ev < lEvaluator.size(); ev++)
		{
			message(lEvaluator.at(ev), iter, "evaluating random move (apply, revert and moveCost).");

			Evaluation<> e = lEvaluator[ev]->evaluate(s);

			string moveFrom = "Move ";
			moveFrom.append(move.id());
			// not printing NS for single Move test
			if (ns)
			{
				moveFrom.append(" from: ");
				moveFrom.append(ns->id());
				moveFrom.append(" toString: ");
				moveFrom.append(ns->toString());
			}

			if (verbose)
				move.print();

			message(moveFrom, iter, "testing reverse.");

			Timer t_clone;
			CopySolution<R,ADS>& sOriginal = s.clone(); // remove if not verbose
			timeSamples.timeCloneSolution.push_back(t_clone.inMilliSecs());

			Timer tMovApply;
			uptr<Move<XES, XEv, XES>> rev = move.apply(s);
			if ((!move.hasReverse() && rev) || (move.hasReverse() && !rev))
			{
				errormsg(moveFrom, CMERR_MOVE_HASREVERSE, "CMERR_MOVE_HASREVERSE", iter, " conflict between apply result and hasReverse()");
				return false;
			}

			timeSamples.timeNSApply[id_ns].push_back(tMovApply.inMilliSecs());

			Timer t_clone2;
			CopySolution<R,ADS>& sNeighbor = s.clone(); // remove if not verbose
			timeSamples.timeCloneSolution.push_back(t_clone2.inMilliSecs());
			// ===================== tests with ADSManager ======================

			// DEPRECATED ADSManager
#if 1
			if (adsMan)
			{
				message(lEvaluator.at(ev), -1, "testing ADS.");

				ADS ads(sNeighbor.getADS()); // copy
				Timer ts_ds;
				adsMan->initializeADS(sNeighbor.getR(), sNeighbor.getADS());
				timeSamples.timeInitializeADS[0].push_back(ts_ds.inMilliSecs());

				if (!adsMan->compareADS(ads, sNeighbor.getADS()))
				{
					cout << "checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from move => ";
					move.print();
					cout << "S (sOriginal.getADS()): " << endl;
					adsMan->printADS(sOriginal.getADS());
					cout << "WRONG: (sNeighbor.getADS())" << endl;
					adsMan->printADS(ads);
					cout << "RIGHT (re-initialized): " << endl;
					adsMan->printADS(sNeighbor.getADS());
					return false;
				}
			}
#endif

			Timer te;
			Evaluation<> e_rev = lEvaluator.at(ev)->evaluate(s);
			timeSamples.fullTimeEval[ev].push_back(te.inMilliSecs());

			Timer tMovRevApply;
			uptr<Move<XES, XEv, XES>> ini = nullptr;
			if (rev)
				ini = rev->apply(s);
			timeSamples.timeNSApply[id_ns].push_back(tMovRevApply.inMilliSecs());
			// ===================== tests with ADSManager ======================

#if 1
			if (adsMan)
			{
				message(lEvaluator.at(ev), -1, "testing ADS.");

				ADS ads(s.getADS()); // copy
				Timer ts_ds2;
				adsMan->initializeADS(s.getR(), s.getADS());
				timeSamples.timeInitializeADS[0].push_back(ts_ds2.inMilliSecs());

				if (!adsMan->compareADS(ads, s.getADS()))
				{
					cout << "checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from reverse move => ";
					Component::safe_print(rev.get());
					cout << "S (sOriginal.getADS()): " << endl;
					adsMan->printADS(sOriginal.getADS());
					cout << "WRONG (s.getADS()): " << endl;
					adsMan->printADS(ads);
					cout << "RIGHT (re-initialized): " << endl;
					adsMan->printADS(s.getADS());
					return false;
				}
			}
#endif

			// go back by copy (if necessary!)
			if (!rev)
				s = sOriginal;

			delete &sOriginal;

			Timer te2;
			Evaluation<> e_ini = lEvaluator.at(ev)->evaluate(s);
			timeSamples.fullTimeEval[ev].push_back(te2.inMilliSecs());

			if (ini && (*ini != move))
			{
				errormsg(moveFrom, CMERR_MOVE_EQUALS, "CMERR_MOVE_EQUALS", iter, "reverse of reverse is not the original move!");
				move.print();
				cout << "move: ";
				move.print();
				cout << "rev: ";
				Component::safe_print(rev.get());
				cout << "ini (reverse of rev): ";
				Component::safe_print(ini.get());

				return false;
			}

			message(lEvaluator.at(ev), iter, "testing reverse value.");

			//if (EVALUATION_ABS(e_ini.evaluation() - e.evaluation()) >= optframe::get_numeric_zero<evtype>())
			if (!optframe::numeric_is_zero(e_ini.evaluation() - e.evaluation()))
			{
				errormsg(moveFrom, CMERR_MOVE_REVREV_VALUE, "CMERR_MOVE_REVREV_VALUE", iter, "reverse of reverse has a different evaluation value!");
				move.print();
				cout << "move: ";
				move.print();
				cout << "original: ";
				e.print();
				cout << "reverse of reverse:";
				e_ini.print();

				return false;
			}

			// =============================================================

			message(lEvaluator.at(ev), iter, "testing move cost.");

			// ==============

			evtype revCost = e_rev.evaluation() - e.evaluation();
			message(lEvaluator.at(ev), iter, "revCost calculated!");

			Timer tMoveCostApply;
			///MoveCost<>* mcSimpleCost = lEvaluator[ev]->moveCostComplete(move, s);
         op<Evaluation<>> mcSimpleCost = lEvaluator[ev]->moveCostComplete(move, s);
			//evtype simpleCost = mcSimpleCost->cost();
         evtype simpleCost = mcSimpleCost->evaluation();
			//delete mcSimpleCost;
			message(lEvaluator.at(ev), iter, "simpleCost calculated!");
			timeSamples.timeNSCostApply[id_ns].push_back(tMoveCostApply.inMilliSecs());

         // TODO: fix ABS with is_zero pattern
			//if (EVALUATION_ABS(revCost - simpleCost) >= optframe::get_numeric_zero<evtype>())
			if (!optframe::numeric_is_zero(revCost - simpleCost))
			{
				errormsg(moveFrom, CMERR_MOVE_REVSIMPLE, "CMERR_MOVE_REVSIMPLE", iter, "difference between revCost and simpleCost");
				cout << "move: ";
				move.print();
				cout << "revCost = " << revCost << endl;
				cout << "simpleCost = " << simpleCost << endl;
				return false;
			}

         pair<S, XEv> se(s, e); // TODO: check if copy is really good here.

			// ==============
			// fasterCost
			Timer tMoveCostApplyDelta;
			Evaluation<> evBeginFasterCost(e);
			uptr<Move<XES, XEv, XES>> rev1 = lEvaluator[ev]->applyMoveReevaluate(move, se);
			evtype e_end1 = e.evaluation();
			// TODO: check outdated status
			// TODO: if(e.outdated), it means that user did not implement Move::applyMoveReevaluate(e,R,ADS)!
//			Move<XES, XEv, XES>& ini1 = *lEvaluator[ev]->applyMoveReevaluate(e, rev1, s);
			uptr<Move<XES, XEv, XES>> ini1 = rev1->apply(s);
			e = std::move(evBeginFasterCost);
			evtype e_ini1 = e.evaluation();
			timeSamples.timeNSCostApplyDelta[id_ns].push_back(tMoveCostApplyDelta.inMilliSecs());

			//delete &rev1;
			//delete &ini1;

			evtype fasterCost = e_end1 - e_ini1;
			message(lEvaluator.at(ev), iter, "fasterCost calculated!");

			//if (EVALUATION_ABS(revCost - fasterCost) >= optframe::get_numeric_zero<evtype>())
			if (!optframe::numeric_is_zero(revCost - fasterCost))
			{
				errormsg(moveFrom, CMERR_MOVE_REVFASTER, "CMERR_MOVE_REVFASTER", iter, "difference between revCost and fasterCost!");
				cout << "move: ";
				move.print();
				cout << "revCost = " << revCost << endl;
				cout << "fasterCost = " << fasterCost << endl;
				cout << "e = " << e.evaluation() << endl;
				cout << "e_rev = " << e_rev.evaluation() << endl;
				return false;
			}
			// ==============

			// ==============
			// real fasterCost
			Timer tMoveCostApplyRealDelta;
			bool oldAllowCostsStatus = lEvaluator[ev]->getAllowCosts();
			lEvaluator[ev]->setAllowCosts(false);
         
			//MoveCost<>* mcRealFasterCost = lEvaluator[ev]->moveCost(move, se);
         op<Evaluation<>> mcRealFasterCost = lEvaluator[ev]->moveCost(move, se);

			lEvaluator[ev]->setAllowCosts(oldAllowCostsStatus);
			//evtype realFasterCost = mcRealFasterCost->cost();
         evtype realFasterCost = mcRealFasterCost->evaluation();
			//delete mcRealFasterCost;
			message(lEvaluator.at(ev), iter, "realFasterCost calculated!");
			timeSamples.timeNSCostApplyRealDelta[id_ns].push_back(tMoveCostApplyRealDelta.inMilliSecs());

			message(lEvaluator.at(ev), iter, "realfasterCost calculated!");

			//if (EVALUATION_ABS(revCost - realFasterCost) >= optframe::get_numeric_zero<evtype>())
			if (!optframe::numeric_is_zero(revCost - realFasterCost))
			{
				errormsg(moveFrom, CMERR_MOVE_REALREVFASTER, "CMERR_MOVE_REALREVFASTER", iter, "difference between revCost and realfasterCost!");
				cout << "move: ";
				move.print();
				cout << "revCost = " << revCost << endl;
				cout << "simpleCost = " << realFasterCost << endl;
				return false;
			}
			// ==============

			Timer tMoveCost;
			///MoveCost<>* cost = nullptr;
         op<Evaluation<>> cost = nullopt;

			if (lEvaluator[ev]->getAllowCosts())
				cost = move.cost(se, false);

			message(lEvaluator.at(ev), iter, "cost() calculated!");

			if (cost && !cost->isEstimated())
				timeSamples.timeNSCost[id_ns].push_back(tMoveCost.inMilliSecs());

			if (cost && cost->isEstimated())
			{
				timeSamples.timeNSEstimatedCost[id_ns].push_back(tMoveCost.inMilliSecs());
				//if (cost->cost() > revCost)
            if (cost->evaluation() > revCost)
					timeSamples.overestimate = true;
				//if (cost->cost() < revCost)
            if (cost->evaluation() < revCost)
					timeSamples.underestimate = true;
			}

			if (cost && !cost->isEstimated())
			{
				//double cValue = cost->cost();
            double cValue = cost->evaluation();
				//if (EVALUATION_ABS(revCost - cValue) >= optframe::get_numeric_zero<evtype>())
				if (!optframe::numeric_is_zero(revCost - cValue))
				{
					errormsg(moveFrom, CMERR_MOVE_COST, "CMERR_MOVE_COST", iter, "difference between expected cost and cost()");
					cout << "move: ";
					move.print();
					cout << "expected cost: (e' - e) =\t" << revCost << endl;
					cout << "cost() =\t" << cValue << endl;
					cout << "e: \t obj:" << e.getObjFunction() << "\t inf: " << e.getInfMeasure() << " \t total:" << e.evaluation() << endl;
					cout << "e':\t obj:" << e_rev.getObjFunction() << "\t inf:" << e_rev.getInfMeasure() << " \t total:" << e_rev.evaluation() << endl;
					//cout << "e+cost():\t obj:" << e.getObjFunction() + cost->getObjFunctionCost() << "\t inf:" << e.getInfMeasure() + cost->getInfMeasureCost() << "\t total:" << e.evaluation() + cost->cost() << endl;
               cout << "e+cost():\t obj:" << e.getObjFunction() + cost->getObjFunction() << "\t inf:" << e.getInfMeasure() + cost->getInfMeasure() << "\t total:" << e.evaluation() + cost->evaluation() << endl;
					cout << "s: ";
					s.print();
					cout << "s': ";
					sNeighbor.print();
					cout << "move: ";
					move.print();
					cout << "==============" << endl;
					cout << "  GOOD LUCK!  " << endl;
					cout << "==============" << endl;
					return false;
				}

				// testing double move costs! (for MoveCost betterThan)
				if (ns) // not testing for single Move
				{
					// TODO: consider that randomMove can be null
					uptr<Move<XES, XEv, XES>> move2 = ns->randomMove(s);
					if (verbose)
					{
						cout << "testing double move!" << endl;
						move2->print();
					}

					if (!move2->canBeApplied(s))
					{
						if (verbose)
						{
							cout << "double move cannot be applied: ";
							move2->print();
						}
					}
					else
					{
						//MoveCost<>* cost2 = nullptr;
                  op<Evaluation<>> cost2 = nullopt;
						if (lEvaluator[ev]->getAllowCosts())
						{
							cost2 = move2->cost(se, false);
							if (cost2)
							{
								lEvaluator[ev]->betterThan(*cost, *cost2);
								//delete cost2;
							}
						}
					}

				}
				// finish double cost test

				//delete cost;
			}

			message(lEvaluator.at(ev), iter, "all move costs okay!");

			//if (rev)
			//	delete rev;
			delete &sNeighbor;
			//if (ini)
			//	delete ini;
		}

		//delete &move;  // ONLY IF NECESSARY! DO IT OUTSIDE...

		return true;
	}

	bool run(int iterMax, int nSolNSSeq)
	{
		// ======================================
		//           BEGIN TESTS
		// ======================================

		cout << "---------------------------------------" << endl;
		cout << "tests/(nSolutions*|constructives|)=" << iterMax << " tests(NSSeq)=" << nSolNSSeq << endl;
		cout << "---------------------------------------" << endl;
		cout << "evaluators=" << lEvaluator.size() << endl;
		cout << "constructives=" << lConstructive.size() << endl;
		cout << "moves=" << lMove.size() << endl;
		cout << "ns=" << lNS.size() << endl;
		cout << "nsseq=" << lNSSeq.size() << endl;
		cout << "nsenum=" << lNSEnum.size() << endl;
		// ADSManager is deprecated!
		cout << "DEPRECATED:adsmanager=" << lADSManagerComp.size() << endl;
		cout << "---------------------------------------" << endl << endl;

		// ----------------
		// read evaluators
		// ----------------

		vector<Evaluator<S, XEv, XES>*> evaluators;
		for (unsigned ev = 0; ev < lEvaluator.size(); ev++)
			evaluators.push_back(lEvaluator[ev]);

		adsMan = nullptr;
		if (lADSManagerComp.size() > 0)
		{
			adsMan = lADSManagerComp[0];

			if (lADSManagerComp.size() > 1)
				cout << " checkcommand warning: more than 1 ADSManager (" << lADSManagerComp.size() << ")" << endl;
		}

		TimeCheckWithSamples timeSamples;
		timeSamples.timeNSApply = vector<vector<double> >(lNS.size());
		timeSamples.timeNSCostApply = vector<vector<double> >(lNS.size());
		timeSamples.timeNSCostApplyDelta = vector<vector<double> >(lNS.size());
		timeSamples.timeNSCostApplyRealDelta = vector<vector<double> >(lNS.size());
		timeSamples.timeNSCost = vector<vector<double> >(lNS.size());
		timeSamples.timeNSEstimatedCost = vector<vector<double> >(lNS.size());
		timeSamples.errorNSEstimatedCost = vector<vector<double> >(lNS.size());
		timeSamples.fullTimeEval = vector<vector<double> >(evaluators.size());
		timeSamples.timeReeval = vector<vector<double> >(evaluators.size());
		timeSamples.timeConstructive = vector<vector<double> >(lConstructive.size());
		timeSamples.overestimate = timeSamples.underestimate = false;

		// ----------------------------------------------------------------------------------------
		// generate 'iterMax' OptFrame:Solution for each OptFrame:Constructive and store evaluation
		// ----------------------------------------------------------------------------------------

		vector<CopySolution<R,ADS>*> solutions;
		vector<vector<Evaluation<>*> > evaluations(evaluators.size());

		if (lConstructive.size() > 0)
			cout << "checkcommand  will test " << lConstructive.size() << " constructive components (iterMax=" << iterMax << ")" << endl;
		for (unsigned c = 0; c < lConstructive.size(); c++)
		{
			//Constructive<S>* constructive = lConstructive.at(c);
         InitialSearch<XES>* constructive = lConstructive.at(c);

			cout << "checkcommand: testing Constructive " << c << " => " << constructive->toString();
			cout << endl;

			for (int iter = 1; iter <= iterMax; iter++)
			{
				message(lConstructive.at(c), iter, "generating solution.");

				Timer ts;
				//CopySolution<R,ADS> s = *constructive->generateSolution(10000000);
            op<XES> ps = constructive->initialSearch(StopCriteria<XES>(10000000));
            //CopySolution<R,ADS> s = *constructive->initialSolution(10000000);
            CopySolution<R,ADS> s = ps->first;
				timeSamples.timeConstructive[c].push_back(ts.inMilliSecs());

				if (adsMan)
				{
					Timer ts2;
					adsMan->initializeADS(s.getR(), s.getADS());
					timeSamples.timeInitializeADS[0].push_back(ts2.inMilliSecs());
				}

				solutions.push_back(new CopySolution<R,ADS>(s));

				for (unsigned ev = 0; ev < evaluators.size(); ev++)
				{
					message(lEvaluator.at(ev), iter, "evaluating solution.");
					Timer te;
					Evaluation<> e = evaluators.at(ev)->evaluate(s);
					timeSamples.fullTimeEval[ev].push_back(te.inMilliSecs());

					evaluations.at(ev).push_back(new Evaluation(e));

					if (lEvaluator.at(ev)->betterThan(e, e))
					{
						errormsg(lEvaluator.at(ev)->toString(), CMERR_EV_BETTERTHAN, "CMERR_EV_BETTERTHAN", iter, "error in betterThan(X,X)=true");
						return false;
					}

					if (!lEvaluator.at(ev)->betterOrEquals(e, e))
					{
						errormsg(lEvaluator.at(ev)->toString(), CMERR_EV_BETTEREQUALS, "CMERR_EV_BETTEREQUALS", iter, "error in betterOrEquals(X,X)=false");
						return false;
					}
				}
			}

			cout << "checkcommand: Constructive " << c << " => " << lConstructive.at(c)->id() << " finished." << endl;
			if (verbose)
				cout << endl << endl;
		}

		if (verbose)
			cout << endl << endl;

		// ====================================================================
		// testing Move
		// ====================================================================

		if (lMove.size() > 0)
			cout << "checkcommand  will test given Move components (|Move|=" << lMove.size() << "; numSolutions=" << solutions.size() << ")" << endl;

		for (unsigned p = 0; p < lSolution.size(); p++)
		{
			solutions.push_back(&lSolution[p]->clone());

			for (unsigned ev = 0; ev < evaluators.size(); ev++)
			{
				message(lEvaluator.at(ev), p, "evaluating input solution.");
				Timer te;
				Evaluation<> e = evaluators.at(ev)->evaluate(*lSolution[p]);
				timeSamples.fullTimeEval[ev].push_back(te.inMilliSecs());

				evaluations.at(ev).push_back(new Evaluation(e));
			}
		}

		for (unsigned id_move = 0; id_move < lMove.size(); id_move++)
		{
			shared_ptr<Move<XES, XEv, XES>> pmove = lMove.at(id_move);

			cout << "checkcommand: testing Move " << id_move << " => " << pmove->toString() << endl;

			for (unsigned id_s = 0; id_s < solutions.size(); id_s++)
			{
				if (verbose)
					cout << endl;
				message(lMove.at(id_move).get(), -1, "working on move.");

				CopySolution<R,ADS>& s = *solutions.at(id_s);

				Move<XES, XEv, XES>& move = *pmove;

            XES se = make_pair(s, XEv());

				if (!move.canBeApplied(se))
				{
					if (verbose)
					{
						cout << "move cannot be applied: ";
						move.print();
					}
					continue;
				}

				// 	bool testMoveGeneral(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckSol& timeSol, TimeNS& timeNS)

				if (!testMoveGeneral(-1, nullptr, -1, s, id_s, move, evaluations, timeSamples))
					return false;

				/////delete& move; // TODO NEVER DESTROY THIS OptFrame:Move!
			}

		}

		if (verbose)
			cout << endl << endl;

		// ====================================================================
		// testing NS
		// ====================================================================

		if (lNS.size() > 0)
			cout << "checkcommand  will test " << lNS.size() << " NS components (iterMax=" << iterMax << " Solutions=" << solutions.size() << ")" << endl;

		for (unsigned id_ns = 0; id_ns < lNS.size(); id_ns++)
		{
			NS<XES, XEv>* ns = lNS.at(id_ns);

			cout << "checkcommand: testing NS " << id_ns << " => " << ns->toString();
			cout << endl;

			for (int iter = 1; iter <= iterMax; iter++)
			{
				message(lNS.at(id_ns), iter, "starting tests!");

				for (unsigned id_s = 0; id_s < solutions.size(); id_s++)
				{
					if (verbose)
						cout << endl;

					stringstream ss_msg1;
					ss_msg1 << "generating random move for solution id=" << id_s;
					message(lNS.at(id_ns), iter, ss_msg1.str());

					CopySolution<R,ADS>& s = *solutions.at(id_s);

               XES se = make_pair(s, XEv());

					// TODO: consider that randomMove can be null
					uptr<Move<XES, XEv, XES>> move = ns->randomMove(se);
					if (verbose)
						move->print();

					if (!move->canBeApplied(se))
					{
						if (verbose)
						{
							cout << "move cannot be applied: ";
							move->print();
						}

						continue;
					}

					// EXEC TESTS HERE

					// bool testMoveNS(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, pair<int, double>& timeCloneSolution, vector<pair<int, double> >& timeInitializeADS, vector<pair<int, double> >& fullTimeEval, vector<pair<int, double> >& timeReeval, TimeNS& timeNS)
					// bool testMoveGeneral(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckSol& timeSol, TimeNS& timeNS)

					if (!testMoveGeneral(iter, ns, id_ns, s, id_s, *move, evaluations, timeSamples))
					{
						return false;
					}
				}
			}

			cout << "checkcommand: " << lNS.at(id_ns)->id() << " finished." << endl;
			if (verbose)
				cout << endl << endl;
		}

		// ====================================================================
		// testing NSSeq
		// ====================================================================

		if (lNSSeq.size() > 0)
			cout << "checkcommand  will test " << lNSSeq.size() << " NSSeq components (nSolNSSeq=" << nSolNSSeq << " of numSolutions=" << solutions.size() << ")" << endl;

		vector<vector<int> > vCountMovesSamples(lNSSeq.size());
		vector<vector<int> > vCountValidMovesSamples(lNSSeq.size());

		for (unsigned id_nsseq = 0; id_nsseq < lNSSeq.size(); id_nsseq++)
		{
			NSSeq<XES, XEv, XES>* nsseq = lNSSeq.at(id_nsseq);

			cout << "checkcommand: testing NSSeq " << id_nsseq << " => " << nsseq->toString();
			cout << endl;

			for (int nqs = 1; nqs <= nSolNSSeq; nqs++)
			{
				message(lNSSeq.at(id_nsseq), nqs, "starting tests!");

				int randomIndex = rand() % solutions.size();
				CopySolution<R,ADS>& s = *solutions.at(randomIndex);
				int id_s = randomIndex;

            XES se = make_pair(s, XEv());

				// ===================
				// apply a single move
				// ===================

				// TODO: consider that iterator can be null!
				NSIterator<XES, XEv>& it = *nsseq->getIterator(se);

				int countMovesNSSeq = 0;
				int countValidMovesNSSeq = 0;

				for (it.first(); !it.isDone(); it.next())
				{
					if (verbose)
						cout << endl;
					message(lNSSeq.at(id_nsseq), nqs, "getting current move (NSSeq tests).");

					// TODO: verify if it's not null!
					uptr<Move<XES, XEv, XES>> move = it.current();
					countMovesNSSeq++;

					if (!move->canBeApplied(se))
					{
						if (verbose)
						{
							cout << "move cannot be applied (NSSeq tests): ";
							move->print();
						}
						continue;
					}

					countValidMovesNSSeq++;

					// EXEC MOVES HERE

					//	bool testMoveNSSeq(int iter, NSSeq<R,ADS>* nsseq, int nqs, int id_nsseq, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, pair<int, double>& timeCloneSolution, vector<pair<int, double> >& timeInitializeADS, vector<pair<int, double> >& fullTimeEval, vector<pair<int, double> >& timeReeval, TimeNS& timeNS)
					// 	bool testMoveGeneral(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckSol& timeSol, TimeNS& timeNS)

					if (!testMoveGeneral(nqs, nsseq, id_nsseq, s, id_s, *move, evaluations, timeSamples))
					{
						return false;
					}
				}

				vCountMovesSamples[id_nsseq].push_back(countMovesNSSeq);
				vCountValidMovesSamples[id_nsseq].push_back(countValidMovesNSSeq);
				delete &it;
			}

			cout << "checkcommand: " << lNSSeq.at(id_nsseq)->id() << " finished." << endl;
			if (verbose)
				cout << endl << endl;
		}

		// ====================================================================
		// testing NSEnum
		// ====================================================================

		if (lNSEnum.size() > 0)
			cout << "checkcommand  will test " << lNSEnum.size() << " NSEnum components (nSolNSSeq=" << nSolNSSeq << " of numSolutions=" << solutions.size() << ")" << endl;

		vector<vector<int>> vCountMovesEnumSamples(lNSEnum.size());
		vector<vector<int>> vCountValidMovesEnumSamples(lNSEnum.size());

		vector<vector<int>> vCountIndependentEnumSamples(lNSEnum.size());
		vector<vector<int>> vCountMovePairsEnumSamples(lNSEnum.size());

		for (unsigned id_nsenum = 0; id_nsenum < lNSEnum.size(); id_nsenum++)
		{
			NSEnum<XES, XEv>* nsenum = lNSEnum.at(id_nsenum);

			cout << "checkcommand: testing NSEnum " << id_nsenum << " => " << nsenum->toString();
			cout << endl;

			for (int nqs = 1; nqs <= nSolNSSeq; nqs++)
			{
				message(lNSEnum.at(id_nsenum), nqs, "starting tests!");

				int randomIndex = rand() % solutions.size();
				CopySolution<R,ADS>& s = *solutions.at(randomIndex);
				int id_s = randomIndex;

            XES se = make_pair(s, XEv());

				// ===================
				// apply a single move
				// ===================

				// TODO: consider that iterator can be null!
				NSIterator<XES, XEv>& it = *nsenum->getIterator(se);
				int countMovesNSEnum = 0;
				int countValidMovesNSEnum = 0;

				for (it.first(); !it.isDone(); it.next())
				{
					if (verbose)
						cout << endl;
					message(lNSEnum.at(id_nsenum), nqs, "getting current move (NSEnum tests).");

					// TODO: verify if it's not null!
					Move<XES, XEv, XES>& move = *it.current();
					countMovesNSEnum++;

					if (!move.canBeApplied(se))
					{
						if (verbose)
						{
							cout << "move cannot be applied (NSEnum tests): ";
							move.print();
						}
						delete &move;
						continue;
					}

					countValidMovesNSEnum++;
					// EXEC MOVES HERE

					//	bool testMoveNSSeq(int iter, NSSeq<R,ADS>* nsseq, int nqs, int id_nsseq, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, pair<int, double>& timeCloneSolution, vector<pair<int, double> >& timeInitializeADS, vector<pair<int, double> >& fullTimeEval, vector<pair<int, double> >& timeReeval, TimeNS& timeNS)
					// 	bool testMoveGeneral(int iter, NS<R,ADS>* ns, int id_ns, CopySolution<R,ADS>& s, int id_s, Move<XES, XEv, XES>& move, vector<vector<Evaluation<>*> >& evaluations, TimeCheckSol& timeSol, TimeNS& timeNS)

					if (!testMoveGeneral(nqs, nsenum, id_nsenum, s, id_s, move, evaluations, timeSamples))
					{
						delete &move;
						return false;
					}

					delete &move;
				}

				vCountMovesEnumSamples[id_nsenum].push_back(countMovesNSEnum);
				vCountValidMovesEnumSamples[id_nsenum].push_back(countValidMovesNSEnum);

				delete &it;
			}



			if (checkIndependent)
			{
				cout << "checkcommand: will try to identify independent moves (can take some time... deactivate with 'checkIndependent=false')" << endl;
				// indicate possible independent moves

				// adopting Evaluator 0...
				Evaluator<S>& ev = *evaluators.at(0);
				int countMovePairsEnum = 0;
				int countMoveIndependentEnum = 0;
				// check for independence between moves m1 and m2
				for (int m1 = 0; m1 < int(nsenum->size()) - 1; m1++)
				{
					// slow...
					cout << "checkcommand: independence test for move " << m1 << " / " << nsenum->size() << endl;
					int count_ind_m1 = 0;
					for (int m2 = m1 + 1; m2 < int(nsenum->size()); m2++)
					{
						bool conflict = false;
						// compute another move pair
						countMovePairsEnum++;

						if (!conflict)
						{
							// TODO: increase performance of this method
							for (unsigned sol = 0; sol < solutions.size(); sol++)
							{
								CopySolution<R,ADS>& s = *solutions.at(sol);
                        XES se = make_pair(s, XEv());

								// TODO: verify if return is return is not null!
								uptr<Move<XES, XEv, XES>> move1 = nsenum->indexMove(m1);
								uptr<Move<XES, XEv, XES>> move2 = nsenum->indexMove(m2);
								// moves must be valid
								if (!(move1->canBeApplied(se) && move2->canBeApplied(se)))
									break;
								// move 1 must have reverse
								if (!move1->hasReverse())
								{
									cout << "checkcommand: NSEnum independent check expected reverse move... (deactivate with 'checkIndependent=false')" << endl;
									break;
								}

								// calculate cost for move 2
								///MoveCost<>* cost_m2 = ev.moveCostComplete(move2, s);
                        op<XEv> cost_m2 = ev.moveCostComplete(*move2, se);

								// apply move 1 (consider reverse is not nullptr)
								uptr<Move<XES, XEv, XES>> rev_m1 = move1->apply(se);

								// calculate new cost for move 2
								///MoveCost<>* cost2_m2 = ev.moveCostComplete(move2, s);
                        op<XEv> cost2_m2 = ev.moveCostComplete(*move2, se);

								// return solution to original value and free
								rev_m1->apply(se);
								//delete &rev_m1;
								//delete &move1;
								//delete &move2;

								// if costs differ, moves are not independent
								if (!ev.equals(*cost2_m2, *cost_m2))
								{
									// mark conflict between m1 and m2
									conflict = true;
									//delete cost2_m2;
									//delete cost_m2;
									break;
								}
								//delete cost2_m2;
								//delete cost_m2;
							}

							if (!conflict)
							{
								// if here, m1 'could' be independent from m2
								countMoveIndependentEnum++;
								count_ind_m1++;
								if (verbose)
								{
									cout << "independent(m1=" << m1 << ";m2=" << m2 << ")" << endl;
									nsenum->indexMove(m1)->print(); // TODO: fix leak
									nsenum->indexMove(m2)->print(); // TODO: fix leak
									cout << endl;
								}
							}

						}
					}
					cout << "checkcommand: found " << count_ind_m1 << " independent move pairs." << endl;
				}

				//Aigor - Check if this counter is right - Any example was tested here
				vCountMovePairsEnumSamples[id_nsenum].push_back(countMovePairsEnum);
				vCountIndependentEnumSamples[id_nsenum].push_back(countMoveIndependentEnum);
			}

			cout << "checkcommand: " << lNSEnum.at(id_nsenum)->id() << " finished." << endl;
			if (verbose)
				cout << endl << endl;
		}

		for (unsigned i = 0; i < solutions.size(); i++)
			delete solutions[i];

		for (unsigned i = 0; i < evaluations.size(); i++)
			for (unsigned j = 0; j < evaluations[i].size(); j++)
				delete evaluations[i][j];

		cout << "===============================" << endl;
		cout << "           SUMMARY             " << endl;
		cout << "===============================" << endl << endl;

		printSingleSummarySamples("Solution", timeSamples.timeCloneSolution, "Time to clone a solution");

		printSummarySamples(convertVector(lConstructive), timeSamples.timeConstructive, "Constructive", "testing construction of initial solution");

		if (adsMan)
			printSummarySamples(convertVector(lADSManagerComp), timeSamples.timeInitializeADS, "ADSManager::initializeADS()", "testing lazy initializeADS in solutions");
		else
			cout << endl << "No ADSManager was tested." << endl << endl;

		printSummarySamples(convertVector(lEvaluator), timeSamples.fullTimeEval, "Evaluators", "testing full evaluate(s) of a solution");

		printSummarySamples(convertVector(lNS), timeSamples.timeNSApply, "NS", "testing time of move apply(s)");

		printSummarySamples(convertVector(lNS), timeSamples.timeNSCostApply, "NS", "testing time of cost based on move apply(s)");

		printSummarySamples(convertVector(lNS), timeSamples.timeNSCostApplyDelta, "NS", "testing time of cost based on move apply(e, s)");

		printSummarySamples(convertVector(lNS), timeSamples.timeNSCostApplyRealDelta, "NS", "testing time of real cost based on move apply(e, s) - forcing allowCosts to False");

		printSummarySamples(convertVector(lNS), timeSamples.timeNSCost, "NS", "testing time of move cost()");

		printSummarySamples(convertVector(lNS), timeSamples.timeNSEstimatedCost, "NS", "testing time of move estimatedCost()");

		printSummarySamples(convertVector(lNS), timeSamples.errorNSEstimatedCost, "NS", "testing error (%) of move estimatedCost()");

		printSummarySimpleSamples(convertVector(lNSSeq), vCountMovesSamples, "NSSeq", "counting moves of NSSeq iterator");

		printSummarySimpleSamples(convertVector(lNSSeq), vCountValidMovesSamples, "NSSeq", "counting valid moves of NSSeq iterator");

		printSummarySimpleSamples(convertVector(lNSEnum), vCountMovesEnumSamples, "NSEnum", "counting moves of NSEnum iterator");

		printSummarySimpleSamples(convertVector(lNSEnum), vCountValidMovesEnumSamples, "NSEnum", "counting valid moves of NSEnum iterator");

		printSummarySimpleSamples(convertVector(lNSEnum), vCountMovePairsEnumSamples, "NSEnum", "counting general move pairs for NSEnum");

		printSummarySimpleSamples(convertVector(lNSEnum), vCountIndependentEnumSamples, "NSEnum", "counting independent move pairs for NSEnum");

		cout << "checkcommand: tests finished successfully!" << endl;
		return true;
	}

	template<class T>
	vector<T*> assignVector(const vector<string> lComponents, T* type, HeuristicFactory<S, XEv, XES, X2ES>& factory)
	{
		vector<T*> vComp;
		for (unsigned i = 0; i < lComponents.size(); i++)
		{
			Scanner scan(lComponents.at(i));
			type = nullptr;
			factory.assign(type, scan.nextInt(), scan.next()); // reversed!

			if (!type)
			{
				cout << "checkcommand  warning: nullptr component " << lComponents[i] << "!" << endl;
			}
			else
				vComp.push_back(type);
		}

		return vComp;
	}

	template<class T>
	vector<Component*> convertVector(const vector<T*>& v)
	{
		vector<Component*> vcomp;
		for (unsigned c = 0; c < v.size(); c++)
			vcomp.push_back((Component*) (v[c]));

		return vcomp;
	}


	void printSummarySimpleSamples(const vector<Component*>& vcomp, const vector<vector<int> >& vMoveSamples, string type, string title)
	{
		unsigned nComponents = vMoveSamples.size();
		printf("---------------------------------\n");
		cout << "|" << type << "|=" << nComponents << "\t" << title << endl;
		printf("---------------------------------\n");
		printf("#id\ttitle\t#tests\ttotal\tavg\tstd \n");
		double avg = 0;
		int validValues = 0;
		for (unsigned id = 0; id < nComponents; id++)
		{
			int nSamples = vMoveSamples[id].size();
			if (nSamples > 0)
			{
				vector<double> vMoveSamplesIDDouble(vMoveSamples[id].begin(), vMoveSamples[id].end());
				KahanAccumulation init =
				{ 0 };
				KahanAccumulation kahanSum = accumulate(vMoveSamplesIDDouble.begin(), vMoveSamplesIDDouble.end(), init, KahanSum);
				pair<double, double> avgStd = calculateAvgStd(vMoveSamplesIDDouble);
				printf("#%d\t%s\t%d\t%.4f\t%.4f\t%.4f\n", ((int) id), vcomp[id]->toString().c_str(), nSamples, kahanSum.sum, avgStd.first, avgStd.second);
				avg += avgStd.first;
				validValues++;
			}
			else
				printf("#%d\t%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", ((int) id), vcomp[id]->toString().c_str(), 0);
		}
		printf("---------------------------------\n");
		printf("all\t*\t-\t%.4f\t-\n", (avg / validValues));
		cout << endl;
	}

	void printSingleSummarySamples(string component, const vector<double> vTimeSamples, string title)
	{
		printf("---------------------------------\n");
		cout << component << "\t" << title << endl;
		printf("---------------------------------\n");
		printf("title\t#tests\tavg(ms)\tstd(ms)\n");
		double avg = 0;
		int validValues = 0;
		int nSamples = vTimeSamples.size();
		if (nSamples > 0)
		{
			pair<double, double> avgStd = calculateAvgStd(vTimeSamples);
			printf("%s\t%d\t%.4f\t%.4f\n", component.c_str(), nSamples, avgStd.first, avgStd.second);
			avg += avgStd.first;
			validValues++;
		}
		else
			printf("%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", component.c_str(), 0);
		printf("---------------------------------\n");
		printf("all\t-\t%.4f\t-\n", (avg / validValues));
		cout << endl;
	}

	void printSummarySamples(const vector<Component*>& vcomp, const vector<vector<double> >& vTimeSamples, string type, string title)
	{
		unsigned nTests = vTimeSamples.size();
		printf("---------------------------------\n");
		cout << "|" << type << "|=" << nTests << "\t" << title << endl;
		printf("---------------------------------\n");
		printf("#id\ttitle\t#tests\tavg(ms)\tstd(ms)\n");
		double avg = 0;
		int validValues = 0;
		for (unsigned id = 0; id < nTests; id++)
		{
			int nSamples = vTimeSamples[id].size();
			if (nSamples > 0)
			{
				pair<double, double> avgStd = calculateAvgStd(vTimeSamples[id]);
				printf("#%d\t%s\t%d\t%.4f\t%.4f\n", ((int) id), vcomp[id]->toString().c_str(), nSamples, avgStd.first, avgStd.second);
				avg += avgStd.first;
				validValues++;
			}
			else
				printf("#%d\t%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", ((int) id), vcomp[id]->toString().c_str(), 0);
		}
		printf("---------------------------------\n");
		printf("all\t*\t-\t%.4f\t-\n", (avg / validValues));
		cout << endl;
	}


};

}

#endif /* OPTFRAME_CHECK_COMMAND_HPP_ */
