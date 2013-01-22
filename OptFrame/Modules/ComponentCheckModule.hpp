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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentCheckModule: public OptFrameModule<R, ADS, DS>
{
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
		return "component.check OptFrame:Constructive[] OptFrame:Evaluator[] OptFrame:NS[] OptFrame:NS:NSSeq[] OptFrame:NS:NSSeq:NSEnum[] iterMax";
	}

	void message(string component, int iter, string text)
	{
		cout << "module " << id() << " iter: " << iter << " testing component '" << component << "' => " << text << endl;
	}

	void error(string text)
	{
		cout << "module " << id() << " error: " << text << endl;
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		cout << "check: " << input << endl;
		Scanner scanner(input);


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


		// -------------------
		//     iterMax
		// -------------------

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
		int iterMax = scanner.nextInt();


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

		// ----------------------------------------------------------------------------------------
		// generate 'iterMax' OptFrame:Solution for each OptFrame:Constructive and store evaluation
		// ----------------------------------------------------------------------------------------

		vector<Solution<R, ADS>* > solutions;
		vector<vector<Evaluation<DS>*> > evaluations(evaluators.size());

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

				Solution<R, ADS>& s = constructive->generateSolution();

				solutions.push_back(&s);

				for(unsigned ev=0; ev<evaluators.size(); ev++)
				{
					message(lEvaluator.at(ev), iter, "evaluating solution.");
					Evaluation<DS>& e = evaluators.at(ev)->evaluate(s);

					evaluations.at(ev).push_back(&e);
				}
			}

			cout << endl << endl;
		}

		cout << endl << endl;

		cout << "module " << id() << " will test NS components (iterMax=" << iterMax << "; numSolutions=" << solutions.size() << ")" << endl;
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
					cout << endl;
					message(lNS.at(id_ns), iter, "generating random move.");

					Solution<R, ADS>& s = *solutions.at(id_s);

					Move<R, ADS, DS>& move = ns->move(s);

					if(!move.canBeApplied(s))
						continue;

					for(unsigned ev=0; ev<evaluators.size(); ev++)
					{
						message(lEvaluator.at(ev), iter, "evaluating random move (apply, revert and moveCost).");

						string moveFrom = "Move ";
						moveFrom.append(move.id());
						moveFrom.append(" from: ");
						moveFrom.append(ns->id());
						move.print();

						message(moveFrom, iter, "testing reverse.");
						Move<R, ADS, DS>& rev = move.apply(s);
						Evaluation<DS>& e_rev = evaluators.at(ev)->evaluate(s);
						Move<R, ADS, DS>& ini = rev.apply(s);
						Evaluation<DS>& e_ini = evaluators.at(ev)->evaluate(s);

						if(ini != move)
						{
							error("reverse of reverse is not the original move!");
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

						// fasterCost
						Move<R, ADS, DS>& rev1 = evaluators[ev]->applyMove(e, move, s);
						double e_end1 = e.evaluation();
						Move<R, ADS, DS>& ini1 = evaluators[ev]->applyMove(e, rev1, s);
						double e_ini1 = e.evaluation();

						delete& rev1;
						delete& ini1;

						double fasterCost = e_end1 - e_ini1;

						pair<double, double>* cost = move.cost(e.getDS(), s.getR(), s.getADS());

						if(abs(revCost - simpleCost) > 0.0001)
						{
							error("difference between revCost and simpleCost");
							printf("revCost = %.4f\n", revCost);
							printf("simpleCost = %.4f\n", simpleCost);
							return false;
						}

						if(abs(revCost - fasterCost) > 0.0001)
						{
							error("difference between revCost and fasterCost");
							printf("revCost = %.4f\n", revCost);
							printf("fasterCost = %.4f\n",  fasterCost);
							printf("e = %.4f\n", e.evaluation());
							printf("e_rev = %.4f\n", e_rev.evaluation());
							return false;
						}

						if(cost)
						{
							double cValue = cost->first+cost->second;
							if(abs(revCost - cValue) > 0.0001)
							{
								error("difference between revCost and cost()");
								printf("revCost = %.4f\n", revCost);
								printf("cost() = %.4f\n", cValue);
								printf("e = %.4f\n", e.evaluation());
								printf("e_rev = %.4f\n", e_rev.evaluation());
								cout << "move: ";
								move.print();
								return false;
							}
						}

						message(lEvaluator.at(ev), iter, "all move costs okay!");
					}
				}
			}

			cout << endl << endl;
		}



		cout << "Tests finished successfully!" << endl;
		return true;
	}

};

#endif /* CHECKMODULE_HPP_ */
