// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

template<class R, class M>
class CheckModule: public OptFrameModule<R, M>
{
public:
	string id()
	{
		return "check";
	}
	string usage()
	{
		return "check [initsol id | loadsol id] evaluator ns_seq_list";
	}

	void run(vector<OptFrameModule<R, M>*>& all_modules, HeuristicFactory<R, M>* factory, map<string, string>* dictionary, string input)
	{
		cout << "check: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string sol = scanner.next();

		if ((sol != "initsol") && (sol != "loadsol"))
		{
			cout << "First parameter must be either 'initsol' or 'loadsol'!" << endl;
			cout << "Usage: " << usage() << endl;
			return;
		}

		string id = scanner.next();

		Solution<R>* s = NULL;

		if (sol == "loadsol")
		{
			Scanner s2(sol + " " + id);
			s = &factory->read_loadsol(&s2)->clone();
		}

		if (sol == "initsol")
		{
			Scanner s2(sol + " " + id);
			cout << "Step 1: Testing solution generator... ";
			InitialSolution<R>* initsol = factory->read_initsol(&s2);
			s = &initsol->generateSolution();
			if (!s)
			{
				cout << "NULL Solution. [Failed]" << endl;
				return;
			}
			cout << "[Ok]" << endl;
		}

		Evaluator<R, M>* eval = factory->read_ev(&scanner);
		vector<NS<R, M>*> ns_list = factory->read_ns_list(&scanner);
		vector<NSSeq<R, M>*> ns_seq_list;
		for (int i = 0; i < ns_list.size(); i++)
			ns_seq_list.push_back((NSSeq<R, M>*) ns_list[i]);

		cout << endl;

		// ==========================================================
		// ==========================================================

		cout << "Step 2: Testing Solution methods" << endl;

		cout << "2.1 - Representation [Ok]" << endl;

		Solution<R>* s2 = &s->clone();
		if (&s2->getR() == &s->getR())
		{
			cout << "Error! Solution has the SAME representation object. Maybe a pointer-based representation?" << endl;
			return;
		}
		cout << "2.2 - Representation Copy [Ok]" << endl;

		delete s2;

		int numTests = 5;
		double totalCopyTime = 0;
		Timer* timer = new Timer(false);

		while (totalCopyTime < 1.0)
		{
			numTests *= 2;
			for (int i = 0; i < numTests; i++)
			{
				s2 = &s->clone();
				delete s2;
			}

			totalCopyTime = timer->now();
			delete timer;
			timer = new Timer(false);
		}

		delete timer;

		double tPerCopy = totalCopyTime / numTests;

		cout << "2.3 - Results" << endl;
		cout << "Number of tests: " << numTests << endl;
		cout << "Total test time: " << totalCopyTime << " seconds" << endl;
		cout << "Time per copy: " << tPerCopy << " seconds" << endl;
		cout << "Printing solution: " << endl;
		s->print();
		cout << "clone() and print() methods [Ok]" << endl;

		cout << endl;

		// ==========================================================
		// ==========================================================

		cout << "Step 3: Testing evaluation methods" << endl;

		cout << "3.1 - Evaluate ";
		Evaluation<M>* e = &eval->evaluate(*s);
		cout << "[Ok]" << endl;

		cout << "3.2 - Evaluation Value = " << e->evaluation() << " ";
		cout << "[Ok]" << endl;

		cout << "3.3 - Evaluation print()" << endl;
		e->print();
		cout << "[Ok]" << endl;

		cout << "3.4 - Re-evaluation" << endl;
		eval->evaluate(*e, *s);
		e->print();
		delete e;
		cout << "[Ok]" << endl;

		numTests = 5;
		double totalEvalTime = 0;
		timer = new Timer(false);

		while (totalEvalTime < 1.0)
		{
			numTests *= 2;
			for (int i = 0; i < numTests; i++)
			{
				e = &eval->evaluate(*s);
				delete e;
			}

			totalEvalTime = timer->now();
			delete timer;
			timer = new Timer(false);
		}

		double tPerEval = totalEvalTime / numTests;

		cout << "3.5 - Evaluation Results" << endl;
		cout << "Number of tests: " << numTests << endl;
		cout << "Total test time: " << totalEvalTime << " seconds" << endl;
		cout << "Time per evaluation: " << tPerEval << " seconds" << endl;

		cout << endl;

		// ==========================================================
		// ==========================================================

		cout << "Step 4: Testing neighborhoods (" << ns_seq_list.size() << ")" << endl;

		/*
		cout << "4.1 - Number of neighbors " << endl;
		for (int i = 0; i < ns_seq_list.size(); i++)
		{
			cout << "neighborhood " << i << ": ";
			ns_seq_list[i]->print();

			double s_total = 0;
			double s_applied = 0;

			ns_seq_list[i]->init(s);

			while (ns_seq_list[i]->hasNext(s))
			{
				Move<R, M>* m = ns_seq_list[i]->next(s);
				s_total++;

				if (m->canBeApplied(s)) // Sum of useful moves
					s_applied++;

				if (m->canBeApplied(s)) // First -> checking moves and reverse moves
				{
					Evaluation<M>* e2 = eval->evaluate(s);

					Move<R, M>* rev = m->apply(s);
					if (!rev)
					{
						cout << "Problem! Reverse move is NULL!" << endl;
						cout << "Move: ";
						m->print();
						return;
					}
					Move<R, M>* new_m = rev->apply(s);
					delete new_m;

					Evaluation<M>* e3 = eval->evaluate(s);

					if (e2->evaluation() != e3->evaluation())
					{
						cout << "Problem with reverse move." << endl;
						cout << "Move: ";
						m->print();
						cout << "Reverse: ";
						rev->print();
						cout << "First evaluation: " << e2->evaluation() << endl;
						cout << "Second evaluation: " << e3->evaluation() << endl;

						delete e2;
						delete e3;
						delete rev;
						delete m;
						return;
					}
				}

				if (m->canBeApplied(s)) // Second -> checking evaluation and reevaluation
				{
					Evaluation<M>* e1 = eval->evaluate(s);
					double f1 = e1->evaluation();

					Move<R, M>* rev = m->apply(e1, s);
					if (!rev)
					{
						cout << "Problem! Reverse move is NULL!" << endl;
						cout << "Move: ";
						m->print();
						return;
					}
					Move<R, M>* new_m = rev->apply(e1, s);
					delete new_m;

					Evaluation<M>* e2 = eval->evaluate(s);

					if (e1->evaluation() != e2->evaluation())
					{
						cout << "Problem with evaluation and reevaluation." << endl;
						cout << "Move: ";
						m->print();
						cout << "Reverse: ";
						rev->print();
						cout << "Reevaluation value: " << e1->evaluation() << endl;
						cout << "Evaluation value: " << e2->evaluation() << endl;

						delete e1;
						delete e2;
						delete rev;
						delete m;
						return;
					}
				}

				delete m;
			}

			cout << "Number of moves: " << s_total << endl;
			cout << "Number of useful moves (can be applied): " << s_applied << endl;
			cout << "Percentage of useful moves: " << (s_applied / s_total) * 100 << "%" << endl;
			if (s_applied < 0.01 * s_total)
				cout << "Warning: useful moves less than 1% of total moves." << endl;
		}

		cout << "4.2 - Best Improvement " << endl;
		for (int i = 0; i < ns_seq_list.size(); i++)
		{
			cout << "neighborhood " << i << ": ";
			s2 = s->clone();
			Solution<R>* s3 = ns_seq_list[i]->bestImprovement(eval, s2);
			s3->print();
			delete s2;
			delete s3;
		}
		cout << "[Ok]" << endl;

		cout << "4.3 - First Improvement " << endl;
		for (int i = 0; i < ns_seq_list.size(); i++)
		{
			cout << "neighborhood " << i << ": ";
			s2 = s->clone();
			Solution<R>* s3 = ns_seq_list[i]->firstImprovement(eval, s2);
			s3->print();
			delete s2;
			delete s3;
		}
		cout << "[Ok]" << endl;
		*/

		// ==========================================================
		// ==========================================================

		cout << "Tests finished successfully!" << endl;
	}

};

#endif /* CHECKMODULE_HPP_ */
