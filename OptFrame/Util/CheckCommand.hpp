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

#ifndef checkcommand_HPP_
#define checkcommand_HPP_

#include "../Timer.hpp"

#include "../OptFrameList.hpp"

#include "../NSEnum.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CheckCommand
{
private:
	bool verbose;

	bool convertNS;

	vector<Evaluator<R, ADS>*> lEvaluator;
	vector<Constructive<R, ADS>*> lConstructive;
	vector<Move<R, ADS>*> lMove;
	vector<NS<R, ADS>*> lNS;
	vector<NSSeq<R, ADS>*> lNSSeq;
	//vector<NSEnum<R, ADS>*> lNSEnum;

public:

	vector<ADSManager<R, ADS>*> lADSManagerComp; // optional

	CheckCommand(bool _verbose = true) :
			verbose(_verbose)
	{
		convertNS = true;
	}

	virtual ~CheckCommand()
	{
	}

	void add(Constructive<R, ADS>& c)
	{
		lConstructive.push_back(&c);
		if (verbose)
			cout << "checkcommand: Constructive " << lConstructive.size() << " added!" << endl;
	}

	void add(Evaluator<R, ADS>& c)
	{
		lEvaluator.push_back(&c);
		if (verbose)
			cout << "checkcommand: Evaluator " << lEvaluator.size() << " added!" << endl;
	}

	void add(Move<R, ADS>& c)
	{
		lMove.push_back(&c);
		if (verbose)
			cout << "checkcommand: Move " << lMove.size() << " added!" << endl;
	}

	void add(NS<R, ADS>& c)
	{
		lNS.push_back(&c);
		if (verbose)
			cout << "checkcommand: NS " << lNS.size() << " added!" << endl;
	}

	void add(NSSeq<R, ADS>& c)
	{
		lNSSeq.push_back(&c);
		if (verbose)
			cout << "checkcommand: NSSeq " << lNSSeq.size() << " added!" << endl;
		if (convertNS)
			add((NS<R, ADS>&) c);
	}

	void add(NSEnum<R, ADS>& c)
	{
		if (convertNS)
			add((NSSeq<R, ADS>&) c);
		else
			cout << "checkcommand warning! NSEnum not used!" << endl;
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

	bool parseBool(string b)
	{
		return b == "true";
	}

	bool run(int iterMax, int nSolNSSeq)
	{
		// ======================================
		//           BEGIN TESTS
		// ======================================

		cout << "---------------------------------------" << endl;
		cout << "tests=" << iterMax << " tests(NSSeq)=" << nSolNSSeq << endl;
		cout << "---------------------------------------" << endl;
		cout << "evaluators=" << lEvaluator.size() << endl;
		cout << "constructives=" << lConstructive.size() << endl;
		cout << "moves=" << lMove.size() << endl;
		cout << "ns=" << lNS.size() << endl;
		cout << "nsseq=" << lNSSeq.size() << endl;
		cout << "adsmanager=" << lADSManagerComp.size() << endl;
		cout << "---------------------------------------" << endl << endl;

		// time to clone a solution
		pair<int, double> timeCloneSolution(0, 0.0);

		ADSManager<R, ADS>* adsMan = NULL;
		if (lADSManagerComp.size() > 0)
		{
			adsMan = lADSManagerComp[0];

			if (lADSManagerComp.size() > 1)
				cout << " checkcommand warning: more than 1 ADSManager (" << lADSManagerComp.size() << ")" << endl;
		}

		vector<pair<int, double> > timeInitializeADS(1, make_pair(0, 0.0));

		// ----------------
		// read evaluators
		// ----------------

		vector<Evaluator<R, ADS>*> evaluators;
		for (unsigned ev = 0; ev < lEvaluator.size(); ev++)
			evaluators.push_back(lEvaluator[ev]);

		vector<pair<int, double> > fullTimeEval(evaluators.size(), make_pair(0, 0.0));
		vector<pair<int, double> > timeReeval(evaluators.size(), make_pair(0, 0.0));

		// ----------------------------------------------------------------------------------------
		// generate 'iterMax' OptFrame:Solution for each OptFrame:Constructive and store evaluation
		// ----------------------------------------------------------------------------------------

		vector<Solution<R, ADS>*> solutions;
		vector<vector<Evaluation*> > evaluations(evaluators.size());

		vector<pair<int, double> > timeConstructive(lConstructive.size(), make_pair(0, 0.0));

		cout << "checkcommand  will test constructive components (iterMax=" << iterMax << ")" << endl;
		for (unsigned c = 0; c < lConstructive.size(); c++)
		{
			Constructive<R, ADS>* constructive = lConstructive.at(c);

			cout << "checkcommand: testing " << constructive->toString();
			cout << endl;

			for (int iter = 1; iter <= iterMax; iter++)
			{
				message(lConstructive.at(c), iter, "generating solution.");

				Timer ts;
				Solution<R, ADS>& s = constructive->generateSolution();
				timeConstructive[c].second += ts.inMilliSecs();
				timeConstructive[c].first++;

				if (adsMan)
				{
					Timer ts2;
					adsMan->initializeADS(s.getR(), s.getADS());
					timeInitializeADS[0].second += ts2.inMilliSecs();
					timeInitializeADS[0].first++;
				}

				solutions.push_back(&s);

				for (unsigned ev = 0; ev < evaluators.size(); ev++)
				{
					message(lEvaluator.at(ev), iter, "evaluating solution.");
					Timer te;
					Evaluation& e = evaluators.at(ev)->evaluate(s);
					fullTimeEval[ev].second += te.inMilliSecs();
					fullTimeEval[ev].first++;

					evaluations.at(ev).push_back(&e);
				}
			}

			cout << "checkcommand: " << lConstructive.at(c)->id() << " finished." << endl;
			if (verbose)
				cout << endl << endl;
		}

		if (verbose)
			cout << endl << endl;

		// ====================================================================
		// testing Move
		// ====================================================================

		cout << "checkcommand  will test given Move components (|Move|=" << lMove.size() << "; numSolutions=" << solutions.size() << ")";
		cout << endl;

		for (unsigned id_move = 0; id_move < lMove.size(); id_move++)
		{
			Move<R, ADS>* pmove = lMove.at(id_move);

			cout << "checkcommand: testing " << pmove->toString();
			cout << endl;

			for (unsigned id_s = 0; id_s < solutions.size(); id_s++)
			{
				if (verbose)
					cout << endl;
				message(lMove.at(id_move), -1, "working on move.");

				Solution<R, ADS>& s = *solutions.at(id_s);

				Move<R, ADS>& move = *pmove;

				if (!move.canBeApplied(s))
				{
					if (verbose)
					{
						cout << "move cannot be applied: ";
						move.print();
					}
					continue;
				}

				for (unsigned ev = 0; ev < evaluators.size(); ev++)
				{
					message(lEvaluator.at(ev), -1, "evaluating move (apply, revert and moveCost).");

					string moveFrom = "Move ";
					moveFrom.append(move.id());

					if (verbose)
						move.print();

					message(moveFrom, -1, "testing reverse.");

					Move<R, ADS>* rev = move.apply(s);

					Timer t_clone;
					Solution<R, ADS>& sNeighbor = s.clone(); // remove if not verbose
					timeCloneSolution.second += t_clone.inMilliSecs();
					timeCloneSolution.first++;

					// ===================== tests with ADSManager ======================

					if (adsMan)
					{
						message(lEvaluator.at(ev), -1, "testing ADS.");

						ADS ads(sNeighbor.getADS()); // copy
						Timer ts_ds;
						adsMan->initializeADS(sNeighbor.getR(), sNeighbor.getADS());
						timeInitializeADS[0].second += ts_ds.inMilliSecs();
						timeInitializeADS[0].first++;

						if (!adsMan->compareADS(ads, sNeighbor.getADS()))
						{
							cout << "checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from move => ";
							move.print();
							return false;
						}

					}

					Evaluation& e_rev = evaluators.at(ev)->evaluate(s);

					Move<R, ADS>* ini = NULL;

					if (rev)
						ini = rev->apply(s);

					// ===================== tests with ADSManager ======================

					if (adsMan)
					{
						message(lEvaluator.at(ev), -1, "testing ADS.");

						ADS ads(s.getADS()); // copy
						Timer ts_ds2;
						adsMan->initializeADS(s.getR(), s.getADS());
						timeInitializeADS[0].second += ts_ds2.inMilliSecs();
						timeInitializeADS[0].first++;

						if (!adsMan->compareADS(ads, s.getADS()))
						{
							cout << " checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from reverse move => ";

							if (rev)
								rev->print();
							else
								cout << "NULL Move" << endl;
							return false;
						}
					}

					Evaluation& e_ini = evaluators.at(ev)->evaluate(s);

					if (ini && (*ini != move))
					{
						error("reverse of reverse is not the original move!");
						move.print();
						cout << "move: ";
						move.print();
						cout << "rev: ";
						Component::safe_print(rev);
						cout << "ini (reverse of rev): ";
						Component::safe_print(ini);

						return false;
					}

					message(lEvaluator.at(ev), -1, "testing reverse value.");
					Evaluation& e = *evaluations.at(ev).at(id_s);

					if (::abs(e_ini.evaluation() - e.evaluation()) > 0.0001)
					{
						error("reverse of reverse has a different evaluation value!");
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

					message(lEvaluator.at(ev), -1, "testing move cost.");

					double revCost = e_rev.evaluation() - e.evaluation();

					MoveCost& mcSimpleCost = evaluators[ev]->moveCost(move, s);
					double simpleCost = mcSimpleCost.cost();
					delete &mcSimpleCost;

					if (abs(revCost - simpleCost) > 0.0001)
					{
						error("difference between revCost and simpleCost");
						move.print();
						printf("revCost = %.4f\n", revCost);
						printf("simpleCost = %.4f\n", simpleCost);
						return false;
					}

					// fasterCost
					Move<R, ADS>& rev1 = evaluators[ev]->applyMove(e, move, s);
					double e_end1 = e.evaluation();
					Move<R, ADS>& ini1 = evaluators[ev]->applyMove(e, rev1, s);
					double e_ini1 = e.evaluation();

					delete &rev1;
					delete &ini1;

					double fasterCost = e_end1 - e_ini1;

					if (abs(revCost - fasterCost) > 0.0001)
					{
						error("difference between revCost and fasterCost");
						move.print();
						printf("revCost = %.4f\n", revCost);
						printf("fasterCost = %.4f\n", fasterCost);
						printf("e = %.4f\n", e.evaluation());
						printf("e_rev = %.4f\n", e_rev.evaluation());
						return false;
					}

					MoveCost* cost = NULL;

					if (evaluators[ev]->getAllowCosts())
						cost = move.cost(e, s.getR(), s.getADS());

					if (cost)
					{
						double cValue = cost->getObjFunctionCost() + cost->getInfMeasureCost();
						if (abs(revCost - cValue) > 0.0001)
						{
							error("difference between expected cost and cost()");
							move.print();
							printf("expected =\t %.4f\n", revCost);
							printf("cost() =\t %.4f\n", cValue);
							printf("==============\n");
							printf("CORRECT VALUES \n");
							printf("==============\n");
							printf("e: \t obj:%.4f \t inf:%.4f \t total:%.4f\n", e.getObjFunction(), e.getInfMeasure(), e.evaluation());
							printf("e':\t obj:%.4f \t inf:%.4f \t total:%.4f\n", e_rev.getObjFunction(), e_rev.getInfMeasure(), e_rev.evaluation());
							cout << "s: ";
							s.print();
							cout << "s': ";
							sNeighbor.print();
							cout << "move: ";
							move.print();
							printf("==============\n");
							printf("  GOOD LUCK!  \n");
							printf("==============\n");
							return false;
						}

						delete cost;
					}

					message(lEvaluator.at(ev), -1, "all move costs okay!");

					delete &rev;
					delete &sNeighbor;
					delete &e_rev;
					delete &ini;
					delete &e_ini;
				}

				/////delete& move; // NEVER DESTROY THIS OptFrame:Move!
			}

		}

		if (verbose)
			cout << endl << endl;

		// ====================================================================
		// testing NS
		// ====================================================================

		cout << "checkcommand  will test NS components (iterMax=" << iterMax << "; numSolutions=" << solutions.size() << ")";
		cout << endl;

		vector<pair<int, double> > timeNSApply(lNS.size(), make_pair(0, 0.0));
		vector<pair<int, double> > timeNSCostApply(lNS.size(), make_pair(0, 0.0));
		vector<pair<int, double> > timeNSCostApplyDelta(lNS.size(), make_pair(0, 0.0));
		vector<pair<int, double> > timeNSCost(lNS.size(), make_pair(0, 0.0));
		vector<pair<int, double> > timeNSEstimatedCost(lNS.size(), make_pair(0, 0.0));
		vector<pair<int, double> > errorNSEstimatedCost(lNS.size(), make_pair(0, 0.0));

		for (unsigned id_ns = 0; id_ns < lNS.size(); id_ns++)
		{
			NS<R, ADS>* ns = lNS.at(id_ns);

			cout << "checkcommand: testing " << ns->toString();
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

					Solution<R, ADS>& s = *solutions.at(id_s);

					Move<R, ADS>& move = ns->move(s);
					if (verbose)
						move.print();

					if (!move.canBeApplied(s))
					{
						if (verbose)
						{
							cout << "move cannot be applied: ";
							move.print();
						}

						delete &move;
						continue;
					}

					for (unsigned ev = 0; ev < evaluators.size(); ev++)
					{
						message(lEvaluator.at(ev), iter, "evaluating random move (apply, revert and moveCost).");

						string moveFrom = "Move ";
						moveFrom.append(move.id());
						moveFrom.append(" from: ");
						moveFrom.append(ns->id());
						moveFrom.append(" toString: ");
						moveFrom.append(ns->toString());

						if (verbose)
							move.print();

						message(moveFrom, iter, "testing reverse.");

						Timer t_clone;
						Solution<R, ADS>& sOriginal = s.clone(); // remove if not verbose
						timeCloneSolution.second += t_clone.inMilliSecs();
						timeCloneSolution.first++;

						Timer tMovApply;
						Move<R, ADS>* rev = move.apply(s);
						timeNSApply[id_ns].second += tMovApply.inMilliSecs();
						timeNSApply[id_ns].first++;

						Timer t_clone2;
						Solution<R, ADS>& sNeighbor = s.clone(); // remove if not verbose
						timeCloneSolution.second += t_clone2.inMilliSecs();
						timeCloneSolution.first++;

						// ===================== tests with ADSManager ======================

						if (adsMan)
						{
							message(lEvaluator.at(ev), -1, "testing ADS.");

							ADS ads(sNeighbor.getADS()); // copy
							Timer ts_ds;
							adsMan->initializeADS(sNeighbor.getR(), sNeighbor.getADS());
							timeInitializeADS[0].second += ts_ds.inMilliSecs();
							timeInitializeADS[0].first++;

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

						Timer te;
						Evaluation& e_rev = evaluators.at(ev)->evaluate(s);
						fullTimeEval[ev].second += te.inMilliSecs();
						fullTimeEval[ev].first++;

						Timer tMovRevApply;
						Move<R, ADS>* ini = NULL;
						if (rev)
							ini = rev->apply(s);
						timeNSApply[id_ns].second += tMovRevApply.inMilliSecs();
						timeNSApply[id_ns].first++;

						// ===================== tests with ADSManager ======================

						if (adsMan)
						{
							message(lEvaluator.at(ev), -1, "testing ADS.");

							ADS ads(s.getADS()); // copy
							Timer ts_ds2;
							adsMan->initializeADS(s.getR(), s.getADS());
							timeInitializeADS[0].second += ts_ds2.inMilliSecs();
							timeInitializeADS[0].first++;

							if (!adsMan->compareADS(ads, s.getADS()))
							{
								cout << "checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from reverse move => ";
								Component::safe_print(rev);
								cout << "S (sOriginal.getADS()): " << endl;
								adsMan->printADS(sOriginal.getADS());
								cout << "WRONG (s.getADS()): " << endl;
								adsMan->printADS(ads);
								cout << "RIGHT (re-initialized): " << endl;
								adsMan->printADS(s.getADS());
								return false;
							}
						}

						delete &sOriginal;

						Timer te2;
						Evaluation& e_ini = evaluators.at(ev)->evaluate(s);
						fullTimeEval[ev].second += te2.inMilliSecs();
						fullTimeEval[ev].first++;

						if (ini && (*ini != move))
						{
							error("reverse of reverse is not the original move!");
							move.print();
							cout << "move: ";
							move.print();
							cout << "rev: ";
							Component::safe_print(rev);
							cout << "ini (reverse of rev): ";
							Component::safe_print(ini);

							return false;
						}

						message(lEvaluator.at(ev), iter, "testing reverse value.");
						Evaluation& e = *evaluations.at(ev).at(id_s);

						if (::abs(e_ini.evaluation() - e.evaluation()) > 0.0001)
						{
							error("reverse of reverse has a different evaluation value!");
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

						double revCost = e_rev.evaluation() - e.evaluation();
						message(lEvaluator.at(ev), iter, "revCost calculated!");

						Timer tMoveCostApply;
						MoveCost& mcSimpleCost = evaluators[ev]->moveCost(move, s);
						double simpleCost = mcSimpleCost.cost();
						delete &mcSimpleCost;
						message(lEvaluator.at(ev), iter, "simpleCost calculated!");
						timeNSCostApply[id_ns].second += tMoveCostApply.inMilliSecs();
						timeNSCostApply[id_ns].first++;

						if (abs(revCost - simpleCost) > 0.0001)
						{
							error("difference between revCost and simpleCost");
							move.print();
							printf("revCost = %.4f\n", revCost);
							printf("simpleCost = %.4f\n", simpleCost);
							return false;
						}

						// fasterCost
						Timer tMoveCostApplyDelta;
						Move<R, ADS>& rev1 = evaluators[ev]->applyMove(e, move, s);
						double e_end1 = e.evaluation();
						Move<R, ADS>& ini1 = evaluators[ev]->applyMove(e, rev1, s);
						double e_ini1 = e.evaluation();
						timeNSCostApplyDelta[id_ns].second += tMoveCostApplyDelta.inMilliSecs();
						timeNSCostApplyDelta[id_ns].first++;

						delete &rev1;
						delete &ini1;

						double fasterCost = e_end1 - e_ini1;
						message(lEvaluator.at(ev), iter, "fasterCost calculated!");

						if (abs(revCost - fasterCost) > 0.0001)
						{
							error("difference between revCost and fasterCost");
							move.print();
							printf("revCost = %.4f\n", revCost);
							printf("fasterCost = %.4f\n", fasterCost);
							printf("e = %.4f\n", e.evaluation());
							printf("e_rev = %.4f\n", e_rev.evaluation());
							return false;
						}

						Timer tMoveCost;
						MoveCost* cost = NULL;

						if (evaluators[ev]->getAllowCosts())
							cost = move.cost(e, s.getR(), s.getADS());

						message(lEvaluator.at(ev), iter, "cost() calculated!");

						if (cost && !cost->isEstimated())
						{
							timeNSCost[id_ns].second += tMoveCost.inMilliSecs();
							timeNSCost[id_ns].first++;
						}

						if (cost && cost->isEstimated())
						{
							timeNSEstimatedCost[id_ns].second += tMoveCost.inMilliSecs();
							timeNSEstimatedCost[id_ns].first++;
						}

						if (cost && !cost->isEstimated())
						{
							double cValue = cost->cost();
							if (abs(revCost - cValue) > 0.0001)
							{
								error("difference between expected cost and cost()");
								move.print();
								printf("expected =\t %.4f\n", revCost);
								printf("cost() =\t %.4f\n", cValue);
								printf("==============\n");
								printf("CORRECT VALUES \n");
								printf("==============\n");
								printf("e: \t obj:%.4f \t inf:%.4f \t total:%.4f\n", e.getObjFunction(), e.getInfMeasure(), e.evaluation());
								printf("e':\t obj:%.4f \t inf:%.4f \t total:%.4f\n", e_rev.getObjFunction(), e_rev.getInfMeasure(), e_rev.evaluation());
								cout << "s: ";
								s.print();
								cout << "s': ";
								sNeighbor.print();
								cout << "move: ";
								move.print();
								printf("==============\n");
								printf("  GOOD LUCK!  \n");
								printf("==============\n");
								return false;
							}

							// testing double move costs! (for MoveCost betterThan)

							Move<R, ADS>& move2 = ns->move(s);
							if (verbose)
							{
								cout << "testing double move!" << endl;
								move2.print();
							}

							if (!move2.canBeApplied(s))
							{
								if (verbose)
								{
									cout << "double move cannot be applied: ";
									move2.print();
								}
							}
							else
							{
								MoveCost* cost2 = NULL;
								if (evaluators[ev]->getAllowCosts())
								{
									cost2 = move2.cost(e, s.getR(), s.getADS());
									if (cost2)
									{
										evaluators[ev]->betterThan(*cost, *cost2);
										delete cost2;
									}
								}
							}

							delete &move2;

							// finish double cost test

							delete cost;
						}

						message(lEvaluator.at(ev), iter, "all move costs okay!");

						delete &rev;
						delete &sNeighbor;
						delete &e_rev;
						delete &ini;
						delete &e_ini;
					}

					delete &move;
				}
			}

			cout << "checkcommand: " << lNS.at(id_ns)->id() << " finished." << endl;
			if (verbose)
				cout << endl << endl;
		}

		// ====================================================================
		// testing NSSeq
		// ====================================================================

		cout << "checkcommand  will test NSSeq components (nSolNSSeq=" << nSolNSSeq << " of numSolutions=" << solutions.size() << ")";
		cout << endl;

		vector<int> vCountMoves(lNSSeq.size());
		vector<int> vCountValidMoves(lNSSeq.size());

		for (unsigned id_nsseq = 0; id_nsseq < lNSSeq.size(); id_nsseq++)
		{
			NSSeq<R, ADS>* nsseq = lNSSeq.at(id_nsseq);

			cout << "checkcommand: testing " << nsseq->toString();
			cout << endl;

			int countMoves = 0;
			int countValidMoves = 0;

			for (int nqs = 1; nqs <= nSolNSSeq; nqs++)
			{
				message(lNSSeq.at(id_nsseq), nqs, "starting tests!");

				int randomIndex = rand() % solutions.size();
				Solution<R, ADS>& s = *solutions.at(randomIndex);

				// ===================
				// apply a single move
				// ===================
				/*

				 bool moveApplied = false;
				 int shaking = rand() % 5 + 1; //Max moves applied in each nSolNSSeq obtained from solutions vector

				 for (int i = 0; i < shaking; i++)
				 {
				 Move<R, ADS>* moveValid = nsseq->validMove(s);

				 if (moveValid != NULL)
				 {
				 delete &moveValid->apply(s);
				 delete moveValid;
				 moveApplied = true;
				 break;
				 }
				 else
				 delete moveValid;
				 }

				 if (!moveApplied)
				 message(lNSSeq.at(id_nsseq), nqs, "Warning. Couldn't apply a move before iterator tests (NSSeq tests).");
				 */

				NSIterator<R, ADS>& it = nsseq->getIterator(s);

				for (it.first(); !it.isDone(); it.next())
				{
					if (verbose)
						cout << endl;
					message(lNSSeq.at(id_nsseq), nqs, "getting current move (NSSeq tests).");

					Move<R, ADS>& move = it.current();
					countMoves++;

					if (!move.canBeApplied(s))
					{
						if (verbose)
						{
							cout << "move cannot be applied (NSSeq tests): ";
							move.print();
						}
						continue;
					}

					countValidMoves++;

					for (unsigned ev = 0; ev < evaluators.size(); ev++)
					{
						message(lEvaluator.at(ev), nqs, "evaluating random move (apply, revert and moveCost) in NSSeq tests.");

						Evaluation& e = evaluators[ev]->evaluate(s);

						string moveFrom = "Move ";
						moveFrom.append(move.id());
						moveFrom.append(" from: ");
						moveFrom.append(nsseq->id());
						moveFrom.append(" toString: ");
						moveFrom.append(nsseq->toString());

						if (verbose)
							move.print();

						message(moveFrom, nqs, "testing reverse.");

						Timer tMovApply;
						Move<R, ADS>* rev = move.apply(s);
						timeNSApply[id_nsseq].second += tMovApply.inMilliSecs();
						timeNSApply[id_nsseq].first++;

						Timer t_clone;
						Solution<R, ADS>& sNeighbor = s.clone(); // remove if not verbose
						timeCloneSolution.second += t_clone.inMilliSecs();
						timeCloneSolution.first++;

						Timer te;
						Evaluation& e_rev = evaluators.at(ev)->evaluate(s);
						fullTimeEval[ev].second += te.inMilliSecs();
						fullTimeEval[ev].first++;

						Timer tMovRevApply;
						Move<R, ADS>* ini = NULL;
						if(rev)
							ini = rev->apply(s);
						timeNSApply[id_nsseq].second += tMovRevApply.inMilliSecs();
						timeNSApply[id_nsseq].first++;

						Timer te2;
						Evaluation& e_ini = evaluators.at(ev)->evaluate(s);
						fullTimeEval[ev].second += te2.inMilliSecs();
						fullTimeEval[ev].first++;

						if (ini && (*ini != move))
						{
							error("reverse of reverse is not the original move!");
							move.print();
							cout << "move: ";
							move.print();
							cout << "rev: ";
							Component::safe_print(rev);
							cout << "ini (reverse of rev): ";
							Component::safe_print(ini);

							return false;
						}

						message(lEvaluator.at(ev), nqs, "testing reverse value (NSSeq tests).");

						if (::abs(e_ini.evaluation() - e.evaluation()) > 0.0001)
						{
							error("reverse of reverse has a different evaluation value!");
							move.print();
							cout << "move: ";
							move.print();
							cout << "original: ";
							e.print();
							cout << "reverse of reverse:";
							e_ini.print();

							return false;
						}

						// ===================== tests with ADSManager ======================

						if (adsMan)
						{
							message(lEvaluator.at(ev), -1, "testing ADS (NSSeq tests).");

							ADS ads(sNeighbor.getADS()); // copy
							Timer ts_ds;
							adsMan->initializeADS(sNeighbor.getR(), sNeighbor.getADS());
							timeInitializeADS[0].second += ts_ds.inMilliSecs();
							timeInitializeADS[0].first++;

							if (!adsMan->compareADS(ads, sNeighbor.getADS()))
							{
								cout << "checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from move => ";
								move.print();
								return false;
							}

							ads = ADS(s.getADS()); // copy
							Timer ts_ds2;
							adsMan->initializeADS(s.getR(), s.getADS());
							timeInitializeADS[0].second += ts_ds2.inMilliSecs();
							timeInitializeADS[0].first++;

							if (!adsMan->compareADS(ads, s.getADS()))
							{
								cout << "checkcommand error: ADS not updated correctly! Compared brand new initializeADS with update from reverse move => ";
								Component::safe_print(rev);
								return false;
							}
						}

						// =============================================================

						message(lEvaluator.at(ev), nqs, "testing move cost (NSSeq tests).");

						double revCost = e_rev.evaluation() - e.evaluation();

						Timer tMoveCostApply;
						MoveCost& mcSimpleCost = evaluators[ev]->moveCost(move, s);
						double simpleCost = mcSimpleCost.cost();
						delete &mcSimpleCost;
						timeNSCostApply[id_nsseq].second += tMoveCostApply.inMilliSecs();
						timeNSCostApply[id_nsseq].first++;

						if (abs(revCost - simpleCost) > 0.0001)
						{
							error("difference between revCost and simpleCost");
							move.print();
							printf("revCost = %.4f\n", revCost);
							printf("simpleCost = %.4f\n", simpleCost);
							return false;
						}

						// fasterCost
						Timer tMoveCostApplyDelta;
						Move<R, ADS>& rev1 = evaluators[ev]->applyMove(e, move, s);
						double e_end1 = e.evaluation();
						Move<R, ADS>& ini1 = evaluators[ev]->applyMove(e, rev1, s);
						double e_ini1 = e.evaluation();
						timeNSCostApplyDelta[id_nsseq].second += tMoveCostApplyDelta.inMilliSecs();
						timeNSCostApplyDelta[id_nsseq].first++;

						delete &rev1;
						delete &ini1;

						double fasterCost = e_end1 - e_ini1;

						if (abs(revCost - fasterCost) > 0.0001)
						{
							error("difference between revCost and fasterCost");
							move.print();
							printf("revCost = %.4f\n", revCost);
							printf("fasterCost = %.4f\n", fasterCost);
							printf("e = %.4f\n", e.evaluation());
							printf("e_rev = %.4f\n", e_rev.evaluation());
							return false;
						}

						Timer tMoveCost;
						MoveCost* cost = NULL;

						if (evaluators[ev]->getAllowCosts())
							cost = move.cost(e, s.getR(), s.getADS());

						if (cost && !cost->isEstimated())
						{
							timeNSCost[id_nsseq].second += tMoveCost.inMilliSecs();
							timeNSCost[id_nsseq].first++;
						}

						if (cost && cost->isEstimated())
						{
							timeNSEstimatedCost[id_nsseq].second += tMoveCost.inMilliSecs();
							timeNSEstimatedCost[id_nsseq].first++;
						}

						if (cost && !cost->isEstimated())
						{
							double cValue = cost->cost();
							if (abs(revCost - cValue) > 0.0001)
							{
								error("difference between expected cost and cost()");
								move.print();
								printf("expected =\t %.4f\n", revCost);
								printf("cost() =\t %.4f\n", cValue);
								printf("==============\n");
								printf("CORRECT VALUES \n");
								printf("==============\n");
								printf("e: \t obj:%.4f \t inf:%.4f \t total:%.4f\n", e.getObjFunction(), e.getInfMeasure(), e.evaluation());
								printf("e':\t obj:%.4f \t inf:%.4f \t total:%.4f\n", e_rev.getObjFunction(), e_rev.getInfMeasure(), e_rev.evaluation());
								cout << "s: ";
								s.print();
								cout << "s': ";
								sNeighbor.print();
								cout << "move: ";
								move.print();
								printf("==============\n");
								printf("  GOOD LUCK!  \n");
								printf("==============\n");
								return false;
							}

							delete cost;
						}

						message(lEvaluator.at(ev), nqs, "all move costs okay (NSSeq tests)!");

						delete &e;

						delete &rev;
						delete &sNeighbor;
						delete &e_rev;
						delete &ini;
						delete &e_ini;
					}

					delete &move;
				}

				delete &it;
			}

			vCountMoves[id_nsseq] += countMoves;
			vCountValidMoves[id_nsseq] += countValidMoves;

			cout << "checkcommand: " << lNSSeq.at(id_nsseq)->id() << " finished." << endl;
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

		printSingleSummary("Solution", timeCloneSolution, "Time to clone a solution");

		printSummary(convertVector(lConstructive), timeConstructive, "Constructive", "testing construction of initial solution");

		if (adsMan)
			printSummary(convertVector(lADSManagerComp), timeInitializeADS, "ADSManager::initializeADS()", "testing lazy initializeADS in solutions");
		else
			cout << endl << "No ADSManager was tested." << endl << endl;

		printSummary(convertVector(lEvaluator), fullTimeEval, "Evaluators", "testing full evaluate(s) of a solution");

		printSummary(convertVector(lNS), timeNSApply, "NS", "testing time of move apply(s)");

		printSummary(convertVector(lNS), timeNSCostApply, "NS", "testing time of cost based on move apply(s)");

		printSummary(convertVector(lNS), timeNSCostApplyDelta, "NS", "testing time of cost based on move apply(e, s)");

		printSummary(convertVector(lNS), timeNSCost, "NS", "testing time of move cost()");

		printSummary(convertVector(lNS), timeNSEstimatedCost, "NS", "testing time of move estimatedCost()");

		printSummary(convertVector(lNS), errorNSEstimatedCost, "NS", "testing error (%) of move estimatedCost()");

		printSummarySimple(convertVector(lNSSeq), vCountMoves, nSolNSSeq, "NSSeq", "counting moves of NSSeq iterator");

		printSummarySimple(convertVector(lNSSeq), vCountValidMoves, nSolNSSeq, "NSSeq", "counting valid moves of NSSeq iterator");

		cout << "checkcommand: tests finished successfully!" << endl;
		return true;
	}

	template<class T>
	vector<T*> assignVector(const vector<string> lComponents, T* type, HeuristicFactory<R, ADS>& factory)
	{
		vector<T*> vComp;
		for (unsigned i = 0; i < lComponents.size(); i++)
		{
			Scanner scan(lComponents.at(i));
			type = NULL;
			factory.assign(type, scan.nextInt(), scan.next()); // reversed!

			if (!type)
			{
				cout << "checkcommand  warning: NULL component " << lComponents[i] << "!" << endl;
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

	void printSingleSummary(string component, const pair<int, double>& value, string title)
	{
		printf("---------------------------------\n");
		cout << component << "\t" << title << endl;
		printf("---------------------------------\n");
		printf("title\t#tests\tavg(ms)\tsum(ms)\n");
		double avg = 0;
		int validValues = 0;
		if (value.first > 0)
		{
			printf("%s\t%d\t%.4f\t%.4f\n", component.c_str(), value.first, (value.second / value.first), value.second);
			avg += (value.second / value.first);
			validValues++;
		}
		else
			printf("%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", component.c_str(), 0);
		printf("---------------------------------\n");
		printf("all\t-\t%.4f\t-\n", (avg / validValues));
		cout << endl;
	}

	void printSummary(const vector<Component*>& vcomp, const vector<pair<int, double> >& values, string type, string title)
	{
		printf("---------------------------------\n");
		cout << "|" << type << "|=" << values.size() << "\t" << title << endl;
		printf("---------------------------------\n");
		printf("#id\ttitle\t#tests\tavg(ms)\tsum(ms)\n");
		double avg = 0;
		int validValues = 0;
		for (unsigned id = 0; id < values.size(); id++)
		{
			if (values[id].first > 0)
			{
				printf("#%d\t%s\t%d\t%.4f\t%.4f\n", ((int) id), vcomp[id]->toString().c_str(), values[id].first, (values[id].second / values[id].first), values[id].second);
				avg += (values[id].second / values[id].first);
				validValues++;
			}
			else
				printf("#%d\t%s\t%d\tUNTESTED OR UNIMPLEMENTED\n", ((int) id), vcomp[id]->toString().c_str(), 0);
		}
		printf("---------------------------------\n");
		printf("all\t*\t-\t%.4f\t-\n", (avg / validValues));
		cout << endl;
	}

	void printSummarySimple(const vector<Component*>& vcomp, const vector<int>& values, int numTests, string type, string title)
	{
		printf("---------------------------------\n");
		cout << "|" << type << "|=" << values.size() << "\t" << title << endl;
		printf("---------------------------------\n");
		printf("#id\ttitle\t#tests\tavg of %d tests\n", numTests);
		double avg = 0;
		int validValues = 0;
		for (unsigned id = 0; id < values.size(); id++)
		{
			if (values[id] > 0)
			{
				printf("#%d\t%s\t%d\t%d\n", ((int) id), vcomp[id]->toString().c_str(), values[id], (values[id] / numTests));
				avg += (values[id] / numTests);
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

#endif /* checkcommand_HPP_ */
