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

#ifndef EXEC_CONSTRUCTIVE_MODULE_HPP_
#define EXEC_CONSTRUCTIVE_MODULE_HPP_

#include "../Command.hpp"
#include "../Constructive.h"

#include "SystemSilentDefineCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentExecConstructiveCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ComponentExecConstructiveCommand()
	{
	}

	string id()
	{
		return "component.exec_constructive";
	}

	string usage()
	{
		return "component.exec_constructive OptFrame:Constructive [output_solution_name]";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "exec_constructive: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		Constructive<R, ADS>* cons;
		factory.assign(cons, scanner.nextInt(), scanner.next()); // reads backwards!

        if(!cons)
        {
            cout << "exec_constructive command: couldn't read OptFrame:Constructive." << endl;
            return false;
        }

		Solution<R, ADS>& sFinal = cons->generateSolution();


		string s_new_id = "";

		int new_id = factory.addComponent(sFinal);

		stringstream str;
		str << Solution<R, ADS>::idComponent() << " " << new_id;
		s_new_id = str.str();

		//cout << "'" << s_new_id << "' added." << endl;

		if (scanner.hasNext())
		{
			string new_name = scanner.next();
			return Command<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, new_name + " " + s_new_id);
		}

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* EXEC_CONSTRUCTIVE_MODULE_HPP_ */
