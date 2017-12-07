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

#ifndef EMPIRICALMODULE_HPP_
#define EMPIRICALMODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class EmpiricalCommand: public Command<R, ADS, DS>
{
public:

	virtual ~EmpiricalCommand()
	{
	}

	string id()
	{
		return "empirical";
	}
	string usage()
	{
		string u = "empirical N T TF BF EVAL METHOD OUTPUTFILE\n WHERE:\n";
		u += "N is the number of tests to be executed;\n";
		u += "T is the timelimit, in seconds, for each test; (0 for no timelimit)\n";
		u += "TF is the target evaluation function value;\n";
		u += "BF is the best known evaluation function value;\n";
		u += "METHOD is the method to be tested with its own parameters;\n";
		u += "OUTPUTFILE is the output file.\n";

		return u;
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return true;
		}

		int n = scanner.nextInt();
		int t = scanner.nextDouble();
		double tf = scanner.nextDouble();
		double bf = scanner.nextDouble();

		pair<SingleObjSearch<R, ADS, DS>*, string> method = factory.createSingleObjSearch(scanner.rest());

		SingleObjSearch<R, ADS, DS>* h = method.first;

		string filename = method.second;

		if (t == 0) // if no timelimit
			t = 1000000000;

		long timelimit = t;

		if (bf == 0) // if best is zero
			bf = 0.00001;

		FILE* file = fopen(filename.c_str(), "a");
		if (!file)
		{
			cout << "Error creating file '" << filename << "'" << endl;
			return true;
		}

		double t_now = 0;
		double fo_now = 0;

		vector<double> time_spent;

		for (int i = 0; i < n; i++)
		{
			long seed = time(nullptr) + i;
			fprintf(file, "%ld\t", seed);

			cout << "Test " << i << " {seed=" << seed << "}... Running";
			Timer t(false);

			pair<Solution<R, ADS>&, Evaluation<DS>&>* result = h->search(timelimit, tf);

			if(!result)
			{
				cout << "EMPIRICAL ERROR!" << endl;
				return false;
			}

			Solution<R, ADS>* s2 = &result->first;

			t_now = t.now();
			fo_now = result->second.evaluation();
			fprintf(file, "%f\t%.3f\t", fo_now, t_now);
			time_spent.push_back(t_now);

			cout << "... Finished! (" << t.now() << "secs.)" << endl;

			delete s2;

			fprintf(file, "\n");
		}

		sort(time_spent.begin(), time_spent.end());

		fprintf(file, "EMPIRICAL PROBABILITY DISTRIBUTION:\ni\ttime(i)\tp(i)=(i-0.5)/N\n0\t0\t0\n");
		for (int i = 1; i <= n; i++)
			fprintf(file, "%i\t%.3f\t%.3f\n", i, time_spent[i - 1], (i - 0.5) / n);

		fprintf(file, "PARAMETERS:%s\n", input.c_str());

		fclose(file);
		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* EMPIRICALMODULE_HPP_ */
