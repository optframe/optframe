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

#ifndef TESTMODULE_HPP_
#define TESTMODULE_HPP_

#include "../Command.hpp"
#include "../Constructive.h"

#include <math.h>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class TestCommand: public Command<R, ADS, DS>
{
public:

	virtual ~TestCommand()
	{
	}

	string id()
	{
		return "test";
	}

	string usage()
	{
		string u = "test N T TF BF EVAL SINGLE_OBJ_SEARCH OUTPUTFILE [solution_name]\n WHERE:\n";
		u += "N is the number of tests to be executed;\n";
		u += "T is the timelimit, in seconds, for each test; (0 for no timelimit)\n";
		u += "TF is the target evaluation function value;\n";
		u += "BF is the best known evaluation function value;\n";
		u += "EVAL is the main evaluator; (e.g. ev 0)\n";
		u += "METHOD is the method to be tested with its own parameters;\n";
		u += "OUTPUTFILE is the output file;\n";
		u += "[solution_name] is a name given to the best found solution (optional).";

		return u;
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		int n = scanner.nextInt();
		int t = scanner.nextDouble();
		double tf = scanner.nextDouble();
		double bf = scanner.nextDouble();

		// Read evaluator
		Component* comp = factory.getNextComponent(scanner);

		Evaluator<R, ADS, DS>* eval = (Evaluator<R, ADS, DS>*) comp;//factory.read_ev(scanner);


		pair<SingleObjSearch<R, ADS, DS>*, string> method = factory.createSingleObjSearch(scanner.rest());

		SingleObjSearch<R, ADS, DS>* h = method.first;

		string rest = method.second;

		Scanner scan_rest(rest);
		//TODO: check \" in filename to allow spaces in the name
		string filename = scan_rest.next();

		if (t == 0) // if no timelimit
			t = 1000000000;

		long timelimit = t;

		if (bf == 0) // if best is zero
			bf = 0.00001;

		FILE* file = fopen(filename.c_str(), "a");
		if (!file)
		{
			cout << "Error creating file '" << filename << "'" << endl;
			return false;
		}

		fprintf(file, "PARAMETERS:%s\n", input.c_str());

		//bool minimization = eval->betterThan(1, 2); // TODO
		Solution<R, ADS>* s_star = nullptr;

		double s_fo_ini = 0;
		double s_t_ini = 0;
		double s_fo_end = 0;
		double s_t_end = 0;
		double min_fo = 1000000000;
		double min_t = 1000000000;
		double max_fo = -1000000000;
		double max_t = -1000000000;

		double t_now = 0;
		double fo_now = 0;
		double variance = 0;

		vector<long double> s_fo_tests(n);

		for (int i = 0; i < n; i++)
		{
			cout << "Test " << i << "... Running";
			Timer t(false);


			pair<Solution<R, ADS>&, Evaluation<DS>&>* result = h->search(timelimit, tf);

			if(!result)
			{
				cout << "ERROR IN TEST MODULE, NO RESULT!" << endl;
				exit(1);
			}

			Solution<R, ADS>* s2 = &result->first;
			t_now = t.now();
			Evaluation< DS > & e2 = result->second;
			fo_now = e2.evaluation();
			delete &e2;
			s_fo_tests.at(i) = fo_now;
			fprintf(file, "%.3f\t%.3f\t", fo_now, t_now);
			s_fo_end += fo_now;
			s_t_end += t_now;

			delete result;

			if (fo_now < min_fo)
				min_fo = fo_now;
			if (t_now < min_t)
				min_t = t_now;
			if (fo_now > max_fo)
				max_fo = fo_now;
			if (t_now > max_t)
				max_t = t_now;

			cout << "... Finished! (" << t.now() << "secs.)" << endl;

			if (!s_star)
				s_star = &s2->clone();
			else if (eval->betterThan(*s2, *s_star))
			{
				delete s_star;
				s_star = &s2->clone();
			}

			delete s2;

			fprintf(file, "\n");
		}

		s_fo_ini /= n;
		s_t_ini /= n;
		s_fo_end /= n;
		s_t_end /= n;

		// calculating variance
		for (int i = 0; i < n; i++)
		{
			variance += pow((s_fo_end - s_fo_tests[i]), 2);
		}
		variance /= (n * 1.0);

		fprintf(file, "AVERAGE:\t%f\t%f\t%f\t%f\n", s_fo_ini, s_t_ini, s_fo_end, s_t_end);
		fprintf(file, "MIN:\t-\t-\t%f\t%f\n", min_fo, min_t);
		fprintf(file, "MAX:\t-\t-\t%f\t%f\n", max_fo, max_t);
		if (min_fo == 0)
			min_fo = 0.00001;
		fprintf(file, "VARIABILITY:\t-\t-\t%f\t-\n", (s_fo_end - min_fo) / min_fo);
		fprintf(file, "VARIANCE: \t-\t-\t%f\t-\n", variance);
		fprintf(file, "STANDARD DEVIATION : \t-\t-\t%f\t-\n", sqrt(variance));
		fprintf(file, "GAP_FROM_BEST:\t%f\t-\t%f\t-\n", (s_fo_ini - bf) / bf, (s_fo_end - bf) / bf);
		fprintf(file, "GAP_FROM_AVG:\t%f\t-\t%f\t-\n", (s_fo_ini - bf) / s_fo_ini, (s_fo_end - bf) / s_fo_end);
		fprintf(file, "IMPROVEMENT:\t-\t-\t%f\t-\n", (bf - min_fo) / bf);
		fprintf(file, "BEST(LIT):\t%f\n", bf);

		fclose(file);

		int new_id = factory.addComponent(*s_star);

		stringstream str;
		str << "OptFrame:Solution " << new_id;
		string s_new_id = str.str();

		cout << "'" << s_new_id << "' added." << endl;

		if (scan_rest.hasNext())
		{
			string new_name = scan_rest.next();
			return Command<R, ADS, DS>::run_module("system.define", all_modules, allFunctions, factory, dictionary, ldictionary, new_name + " " + s_new_id);
		}

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* TESTMODULE_HPP_ */
