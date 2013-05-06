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

#ifndef EXECMODULE_HPP_
#define EXECMODULE_HPP_

#include "../OptFrameModule.hpp"
#include "../Constructive.h"

#include "SystemSilentDefineModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentExecModule: public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~ComponentExecModule()
	{
	}

	string id()
	{
		return "component.exec";
	}

	string usage()
	{
		return "component.exec target_fo timelimit sios_method output_solution_name [spent_time]";
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "exec: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		double target_fo = scanner.nextDouble();
		double timelimit = scanner.nextDouble();

		pair<SingleObjSearch<R, ADS, DS>*, string> method = factory.createSingleObjSearch(scanner.rest());
		scanner = Scanner(method.second);

		// ---

		string s_new_id = "";

		Timer t;
		pair<Solution<R, ADS>&, Evaluation<DS>&>* result = method.first->search(timelimit, target_fo);
		double time = t.inMilliSecs();


		if(!result)
		{
			cout << "NO RESULT! EXEC ERROR!" << endl;
			return false;
		}

		Solution<R, ADS>& sFinal = result->first;

		int new_id = factory.addComponent(sFinal);

		stringstream str;
		str << Solution<R, ADS>::idComponent() << " " << new_id;
		s_new_id = str.str();

		//cout << "'" << s_new_id << "' added." << endl;

		if (scanner.hasNext())
		{
			string new_name = scanner.next();
			if(!OptFrameModule<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, new_name + " " + s_new_id))
				return false;
		}

		if (scanner.hasNext())
		{
			string var_time = scanner.next();
			stringstream ss;
			ss.precision(OptFrameModule<R, ADS, DS>::precision);
			ss << fixed;
			ss << var_time << " " << time;
			if(!OptFrameModule<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str()))
				return false;
		}

		return true;
	}

};

#endif /* EXECMODULE_HPP_ */
