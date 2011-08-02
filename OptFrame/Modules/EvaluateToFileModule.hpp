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

#ifndef EVALUATETOFILEMODULE_HPP_
#define EVALUATETOFILEMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class EvaluateToFileModule: public OptFrameModule<R, M>
{
public:
	string id()
	{
		return "evaluate_to_file";
	}
	string usage()
	{
		string u = "evaluate_to_file ev id loadsol id filename";
		return u;
	}

	void run(vector<OptFrameModule<R, M>*>& all_modules, HeuristicFactory<R, M>* factory, map<string, string>* dictionary, string input)
	{
		cout << "evaluate_to_file: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		Evaluator<R, M>* eval = factory->read_ev(&scanner);

		string sol = scanner.next();

		if (sol != "loadsol")
		{
			cout << "Second parameter must be a 'loadsol'!" << endl;
			cout << "Usage: " << usage() << endl;
			return;
		}

		string id = scanner.next();

		Scanner s2(sol + " " + id);
		Solution<R>* s = factory->read_loadsol(&s2);

		Evaluation<M>* e = &eval->evaluate(*s);
		e->print();

      string filename = scanner.next();

      FILE* fev = fopen(filename.c_str(), "a");
      fprintf(fev, "%.4f\n", e->evaluation());
      fclose(fev);

		delete e;
	}

};

#endif /* EVALUATEMODULE_HPP_ */
