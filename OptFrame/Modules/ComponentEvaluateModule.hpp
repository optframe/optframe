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

#ifndef EVALUATEMODULE_HPP_
#define EVALUATEMODULE_HPP_

#include "../OptFrameModule.hpp"

#include "SystemSilentDefineModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentEvaluateModule: public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~ComponentEvaluateModule()
	{
	}

	string id()
	{
		return "component.evaluate";
	}
	string usage()
	{
		string u = "component.evaluate OptFrame:Evaluator id OptFrame:Solution id [store_value]";
		return u;
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "evaluate: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		Evaluator<R, ADS, DS>* eval = factory.read_ev(scanner);

		string sol = scanner.next();

		if (sol != Solution<R,ADS>::idComponent())
		{
			cout << "Second parameter must be a 'OptFrame:Solution'!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string id = scanner.next();

		Scanner s2(sol + " " + id);
		Solution<R, ADS>* s = NULL;
		factory.readComponent(s, s2);

		Evaluation<DS>* e = &eval->evaluate(*s);

		if(scanner.hasNext())
		{
			string ename = scanner.next();
			stringstream ss;
			ss << ename << " " << e->evaluation();
			delete e;
			return OptFrameModule<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
		}
		else
		{
			e->print();
			delete e;
			return true;
		}

	}

};

#endif /* EVALUATEMODULE_HPP_ */
