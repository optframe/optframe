// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef CHECKMODULE_HPP_
#define CHECKMODULE_HPP_

#include "../OptFrameModule.hpp"
#include "../Util/Timer.hpp"

#include "SystemRequireModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentCheckModule: public OptFrameModule<R, ADS, DS>
{
private:
	bool verbose;

public:

	virtual ~ComponentCheckModule()
	{
	}

	string id()
	{
		return "component.check";
	}

	string usage()
	{
		return "component.check iterMax verbose=false [ OptFrame:Constructive[] OptFrame:Evaluator[] OptFrame:NS[] OptFrame:NS:NSSeq[] OptFrame:NS:NSSeq:NSEnum[] ]";
	}

	void message(string component, int iter, string text)
	{
		if(verbose)
			cout << "module " << id() << " iter: " << iter << " testing component '" << component << "' => " << text << endl;
	}

	void error(string text)
	{
		cout << "module " << id() << " error: " << text << endl;
	}

	bool parseBool(string b)
	{
		return b == "true";
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		cout << "check: " << input << endl;
		Scanner scanner(input);

		// -------------------
		//     iterMax
		// -------------------

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
		int iterMax = scanner.nextInt();


		// -------------------
		//     verbose
		// -------------------

		if (!scanner.hasNext())
			verbose = false;
		else
		{
			string sverbose = scanner.next();
			verbose = parseBool(sverbose);
		}


		// optional parameters taken automatically from the system
		if(!scanner.hasNext())
		{
			// check dependency on 'module.rename' module
			if(!OptFrameModule<R, ADS, DS>::run_module("system.require", allModules, allFunctions, factory, dictionary, ldictionary, "component.list"))
			{
				cout << "error: system.use module depends on 'component.list' module, which is not loaded!" << endl;
				return false;
			}

			OptFrameModule<R, ADS, DS>::undefine("_aux_check_lconstructive", dictionary, ldictionary);
			if(!OptFrameModule<R, ADS, DS>::run_module("component.list", allModules, allFunctions, factory, dictionary, ldictionary, "OptFrame:Constructive _aux_check_lconstructive"))
			{
				cout << "error: reading list of OptFrame:Constructive!" << endl;
				return false;
			}

			OptFrameModule<R, ADS, DS>::undefine("_aux_check_levaluator", dictionary, ldictionary);
			if(!OptFrameModule<R, ADS, DS>::run_module("component.list", allModules, allFunctions, factory, dictionary, ldictionary, "OptFrame:Evaluator _aux_check_levaluator"))
			{
				cout << "error: reading list of OptFrame:Evaluator!" << endl;
				return false;
			}

			OptFrameModule<R, ADS, DS>::undefine("_aux_check_lns", dictionary, ldictionary);
			if(!OptFrameModule<R, ADS, DS>::run_module("component.list", allModules, allFunctions, factory, dictionary, ldictionary, "OptFrame:NS _aux_check_lns"))
			{
				cout << "error: reading list of OptFrame:NS!" << endl;
				return false;
			}

			OptFrameModule<R, ADS, DS>::undefine("_aux_check_lnsseq", dictionary, ldictionary);
			if(!OptFrameModule<R, ADS, DS>::run_module("component.list", allModules, allFunctions, factory, dictionary, ldictionary, "OptFrame:NS:NSSeq _aux_check_lnsseq"))
			{
				cout << "error: reading list of OptFrame:NS:NSSeq!" << endl;
				return false;
			}

			OptFrameModule<R, ADS, DS>::undefine("_aux_check_lnsenum", dictionary, ldictionary);
			if(!OptFrameModule<R, ADS, DS>::run_module("component.list", allModules, allFunctions, factory, dictionary, ldictionary, "OptFrame:NS:NSSeq:NSEnum _aux_check_lnsenum"))
			{
				cout << "error: reading list of OptFrame:NS:NSSeq:NSEnum!" << endl;
				return false;
			}

			scanner = Scanner("_aux_check_lconstructive _aux_check_levaluator _aux_check_lns _aux_check_lnsseq _aux_check_lnsenum");
		}

		//string rest = scanner.rest();
		//cout << "REST: '" << rest << "'" << endl;
		//scanner = Scanner(rest);

		// -------------------
		//    Constructive
		// -------------------

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
		vector<string>  lConstructive;
		vector<string>* p_lConstructive = OptFrameList::readList(ldictionary, scanner);
		if(p_lConstructive)
		{
			lConstructive = vector<string>(*p_lConstructive);
			delete p_lConstructive;
		}
		else
		{
			cout << "module " << id() << " error: couldn't read list of OptFrame:Constructive!" << endl;
			return false;
		}


		// -------------------
		//     Evaluator
		// -------------------

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
		vector<string>  lEvaluator;
		vector<string>* p_lEvaluator = OptFrameList::readList(ldictionary, scanner);
		if(p_lEvaluator)
		{
			lEvaluator = vector<string>(*p_lEvaluator);
			delete p_lEvaluator;
		}
		else
		{
			cout << "module " << id() << " error: couldn't read list of OptFrame:Evaluator!" << endl;
			return false;
		}


		// -------------------
		//        NS
		// -------------------

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
		vector<string>  lNS;
		vector<string>* p_lNS = OptFrameList::readList(ldictionary, scanner);
		if(p_lNS)
		{
			lNS = vector<string>(*p_lNS);
			delete p_lNS;
		}
		else
		{
			cout << "module " << id() << " error: couldn't read list of OptFrame:NS!" << endl;
			return false;
		}


		// -------------------
		//     NSSeq
		// -------------------

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
		vector<string>  lNSSeq;
		vector<string>* p_lNSSeq = OptFrameList::readList(ldictionary, scanner);
		if(p_lNSSeq)
		{
			lNSSeq = vector<string>(*p_lNSSeq);
			delete p_lNSSeq;
		}
		else
		{
			cout << "module " << id() << " error: couldn't read list of OptFrame:NS:NSSeq!" << endl;
			return false;
		}


		// -------------------
		//     NSEnum
		// -------------------

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
		vector<string>  lNSEnum;
		vector<string>* p_lNSEnum = OptFrameList::readList(ldictionary, scanner);
		if(p_lNSEnum)
		{
			lNSEnum = vector<string>(*p_lNSEnum);
			delete p_lNSEnum;
		}
		else
		{
			cout << "module " << id() << " error: couldn't read list of OptFrame:NS:NSSeq:NSEnum!" << endl;
			return false;
		}

		// cleanup auxiliar list definitions
		OptFrameModule<R, ADS, DS>::undefine("_aux_check_lconstructive", dictionary, ldictionary);
		OptFrameModule<R, ADS, DS>::undefine("_aux_check_levaluator", dictionary, ldictionary);
		OptFrameModule<R, ADS, DS>::undefine("_aux_check_lns", dictionary, ldictionary);
		OptFrameModule<R, ADS, DS>::undefine("_aux_check_lnsseq", dictionary, ldictionary);
		OptFrameModule<R, ADS, DS>::undefine("_aux_check_lnsenum", dictionary, ldictionary);


		// ======================================
		//           BEGIN TESTS
		// ======================================

		// ----------------
		// read evaluators
		// ----------------

		vector<Evaluator<R, ADS, DS>* > evaluators;
		for(unsigned ev=0; ev<lEvaluator.size(); ev++)
		{
			Scanner scan(lEvaluator.at(ev));
			Evaluator<R, ADS, DS>* evaluator;
			factory.assign(evaluator, scan.nextInt(), scan.next()); // reversed!

			if(!evaluator)
			{
				cout << "module " << id() << " error: NULL evaluator!" << endl;
				return false;
			}

			evaluators.push_back(evaluator);
		}

		vector< pair<int, double> > fullTimeEval(evaluators.size(), make_pair(0,0.0));

		// ----------------------------------------------------------------------------------------
		// generate 'iterMax' OptFrame:Solution for each OptFrame:Constructive and store evaluation
		// ----------------------------------------------------------------------------------------

		vector<Solution<R, ADS>* > solutions;
		vector<vector<Evaluation<DS>*> > evaluations(evaluators.size());

		vector< pair<int, double> > timeConstructive(lConstructive.size(), make_pair(0,0.0));

		cout << "module " << id() << " will test constructive components (iterMax=" << iterMax << ")" << endl;
		for(unsigned c=0; c<lConstructive.size(); c++)
		{
			Scanner scan(lConstructive.at(c));
			Constructive<R, ADS>* constructive;
			factory.assign(constructive, scan.nextInt(), scan.next()); // reversed!

			if(!constructive)
			{
				cout << "module " << id() << " error: NULL constructive!" << endl;
				return false;
			}

			for(int iter=1; iter<=iterMax; iter++)
			{
				message(lConstructive.at(c), iter, "generating solution.");

				Timer ts;
				Solution<R, ADS>& s = constructive->generateSolution();
				timeConstructive[c].second += ts.inMilliSecs();
				timeConstructive[c].first++;

				solutions.push_back(&s);

				for(unsigned ev=0; ev<evaluators.size(); ev++)
				{
					message(lEvaluator.at(ev), iter, "evaluating solution.");
					Timer te;
					Evaluation<DS>& e = evaluators.at(ev)->evaluate(s);
					fullTimeEval[ev].second += te.inMilliSecs();
					fullTimeEval[ev].first++;

					evaluations.at(ev).push_back(&e);
				}
			}

			if(verbose)
				cout << endl << endl;
		}

		if(verbose)
			cout << endl << endl;

		cout << "module " << id() << " will test NS components (iterMax=" << iterMax << "; numSolutions=" << solutions.size() << ")" << endl;

		vector< pair<int, double> > timeNSApply(lNS.size(), make_pair(0,0.0));

		for(unsigned id_ns=0; id_ns<lNS.size(); id_ns++)
		{
			Scanner scan(lNS.at(id_ns));
			NS<R, ADS, DS>* ns;
			factory.assign(ns, scan.nextInt(), scan.next()); // reversed!

			if(!ns)
			{
				cout << "module " << id() << " error: NULL OptFrame:NS!" << endl;
				return false;
			}

			for(int iter=1; iter<=iterMax; iter++)
			{
				message(lNS.at(id_ns), iter, "starting tests!");

				for(unsigned id_s=0; id_s < solutions.size(); id_s++)
				{
					if(verbose)
						cout << endl;
					message(lNS.at(id_ns), iter, "generating random move.");

					Solution<R, ADS>& s = *solutions.at(id_s);

					Move<R, ADS, DS>& move = ns->move(s);

					if(!move.canBeApplied(s))
					{
                  if(verbose)
                  {
                     cout << "move cannot be applied: ";
                     move.print();
                  }
						continue;
					}

					for(unsigned ev=0; ev<evaluators.size(); ev++)
					{
						message(lEvaluator.at(ev), iter, "evaluating random move (apply, revert and moveCost).");

						string moveFrom = "Move ";
						moveFrom.append(move.id());
						moveFrom.append(" from: ");
						moveFrom.append(ns->id());
						moveFrom.append(" toString: ");
						moveFrom.append(ns->toString());

						if(verbose)
							move.print();

						message(moveFrom, iter, "testing reverse.");

						Timer tMovApply;
						Move<R, ADS, DS>& rev = move.apply(s);
						timeNSApply[id_ns].second += tMovApply.inMilliSecs();
						timeNSApply[id_ns].first++;

						Solution<R, ADS>& sNeighbor = s.clone(); // remove if not verbose

						Timer te;
						Evaluation<DS>& e_rev = evaluators.at(ev)->evaluate(s);
						fullTimeEval[ev].second += te.inMilliSecs();
						fullTimeEval[ev].first++;

						Timer tMovRevApply;
						Move<R, ADS, DS>& ini = rev.apply(s);
						timeNSApply[id_ns].second += tMovRevApply.inMilliSecs();
						timeNSApply[id_ns].first++;

						Timer te2;
						Evaluation<DS>& e_ini = evaluators.at(ev)->evaluate(s);
						fullTimeEval[ev].second += te2.inMilliSecs();
						fullTimeEval[ev].first++;

						if(ini != move)
						{
							error("reverse of reverse is not the original move!");
							move.print();
							cout << "move: ";
							move.print();
							cout << "rev: ";
							rev.print();
							cout << "ini (reverse of rev): ";
							ini.print();

							return false;
						}

						message(lEvaluator.at(ev), iter, "testing reverse value.");
						Evaluation<DS>& e = *evaluations.at(ev).at(id_s);

						if(abs(e_ini.evaluation() - e.evaluation()) > 0.0001)
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

						message(lEvaluator.at(ev), iter, "testing move cost.");

						double revCost = e_rev.evaluation() - e.evaluation();

						double simpleCost = evaluators[ev]->moveCost(move, s);

						if(abs(revCost - simpleCost) > 0.0001)
						{
							error("difference between revCost and simpleCost");
							move.print();
							printf("revCost = %.4f\n", revCost);
							printf("simpleCost = %.4f\n", simpleCost);
							return false;
						}

						// fasterCost
						Move<R, ADS, DS>& rev1 = evaluators[ev]->applyMove(e, move, s);
						double e_end1 = e.evaluation();
						Move<R, ADS, DS>& ini1 = evaluators[ev]->applyMove(e, rev1, s);
						double e_ini1 = e.evaluation();

						delete& rev1;
						delete& ini1;

						double fasterCost = e_end1 - e_ini1;

						if(abs(revCost - fasterCost) > 0.0001)
						{
							error("difference between revCost and fasterCost");
							move.print();
							printf("revCost = %.4f\n", revCost);
							printf("fasterCost = %.4f\n",  fasterCost);
							printf("e = %.4f\n", e.evaluation());
							printf("e_rev = %.4f\n", e_rev.evaluation());
							return false;
						}

						pair<double, double>* cost = move.cost(e, s.getR(), s.getADS());

						if(cost)
						{
							double cValue = cost->first+cost->second;
							if(abs(revCost - cValue) > 0.0001)
							{
								error("difference between revCost and cost()");
								move.print();
								printf("revCost = %.4f\n", revCost);
								printf("cost() = %.4f\n", cValue);
								printf("e = %.4f\n", e.evaluation());
								printf("e_rev = %.4f\n", e_rev.evaluation());
								cout << "s: ";
								s.print();
								cout << "s': ";
								sNeighbor.print();
								cout << "move: ";
								move.print();
								return false;
							}

							delete cost;
						}

						message(lEvaluator.at(ev), iter, "all move costs okay!");

						delete& rev;
						delete& sNeighbor;
						delete& e_rev;
						delete& ini;
						delete& e_ini;
					}

					delete& move;
				}
			}

			if(verbose)
				cout << endl << endl;
		}


		for(unsigned i=0; i<solutions.size(); i++)
			delete solutions[i];

		for(unsigned i=0; i<evaluations.size(); i++)
			for(unsigned j=0; j<evaluations[i].size(); j++)
				delete evaluations[i][j];

		cout << "===============================" << endl;
		cout << "           SUMMARY             " << endl;
		cout << "===============================" << endl << endl;

		printSummary(timeConstructive, "Constructive", "testing construction of initial solution");

		printSummary(fullTimeEval, "Evaluators", "testing full evaluate(s) of a solution");

		printSummary(timeNSApply, "NS", "testing time of move apply(s)");

		cout << "component.check module: tests finished successfully!" << endl;
		return true;
	}

   void printSummary(const vector< pair<int, double> >& values, string type, string title)
   {
      cout << "|"<<type<<"|=" << values.size() << "\t" << title << endl;
      printf("#id\t#tests\tsum(ms)\tavg(ms)\n");
		for(unsigned id=0; id<values.size(); id++)
			printf("#%d\t%d\t%.4f\t%.4f\n", ((int)id), values[id].first, values[id].second, (values[id].second/values[id].first));
		cout << endl;
   }

};

#endif /* CHECKMODULE_HPP_ */
