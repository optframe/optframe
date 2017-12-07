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

#ifndef OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_
#define OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_

#include "../Command.hpp"

#include "SystemUnsafeDefineCommand.hpp"


namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemPreprocessCommand: public Command<R, ADS, DS>
{
public:

	virtual ~SystemPreprocessCommand()
	{
	}

	string id()
	{
		return "system.preprocess";
	}

	string usage()
	{
		return "system.preprocess return_value module_name input";
	}

	Command<R, ADS, DS>* getCommand(vector<Command<R, ADS, DS>*>& modules, string module, string rest)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
		{
			//cout << "run: testing module '" << modules[i]->id() << "'" << endl;
			if (modules[i]->canHandle(module, rest))
				return modules[i];
		}
		//cout << "run: nullptr MODULE! module='" << module << "' rest='" << rest << "'" << endl;
		return nullptr;
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string name = scanner.next();

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string module = scanner.next();

		string inp = scanner.rest();

		Command<R, ADS, DS>* m = getCommand(all_modules, module, "");

		if(!m)
		{
			cout << "preprocess command: nullptr module!" << endl;
			return false;
		}

		string* final = m->preprocess(allFunctions, factory, dictionary, ldictionary, inp);

		if(!final)
			return false;

		stringstream ss;
		ss << name << " " << (*final);

		delete final;

		return Command<R, ADS, DS>::run_module("system.unsafe_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
	}

	// runs raw module without preprocessing
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return new string(input); // disable pre-processing
	}

};

}

#endif /* OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_ */
