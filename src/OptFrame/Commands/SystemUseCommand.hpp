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

#ifndef OPTFRAME_SYSTEM_USE_MODULE_HPP_
#define OPTFRAME_SYSTEM_USE_MODULE_HPP_

#include "../Command.hpp"

#include "SystemRequireCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemUseCommand : public Command<R, ADS, DS>
{
public:

	virtual ~SystemUseCommand()
	{
	}

	string id()
	{
		return "system.use";
	}

	string usage()
	{
		return "system.use prefix -module_name ...";
	}

	bool moduleExists(string moduleName, vector<Command<R, ADS, DS>*>& allCommands)
	{
		for(unsigned i=0; i<allCommands.size(); i++)
			if(allCommands[i]->id() == moduleName)
				return true;
		return false;
	}

	bool functionExists(string functionName, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions)
	{
		for(unsigned i=0; i<allFunctions.size(); i++)
			if(allFunctions[i]->id() == functionName)
				return true;
		return false;
	}

	bool in(string s, vector<string>& vs)
	{
		for(unsigned i=0; i<vs.size(); i++)
			if(vs[i] == s)
				return true;
		return false;
	}


	bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		/*
		// check dependency on 'command.rename' module
		if(!Command<R, ADS, DS>::run_module("system.require", allCommands, allFunctions, factory, dictionary, ldictionary, "command.rename"))
		{
			cout << "error: system.use module depends on 'command.rename' module, which is not loaded!" << endl;
			return false;
		}
		*/

		/*
		// check dependency on 'function.rename' module
		if(!Command<R, ADS, DS>::run_module("system.require", allCommands, allFunctions, factory, dictionary, ldictionary, "function.rename"))
		{
			cout << "error: system.use module depends on 'function.rename' module, which is not loaded!" << endl;
			return false;
		}
		*/

		Scanner scanner(input);

		if(!scanner.hasNext()) // no module
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string prefix = scanner.next();

		vector<string> negative_exceptions;

		while(scanner.hasNext())
		{
			string exception = scanner.next();

			Scanner scanReal(exception);
			scanReal.useSeparators("-");
			string real = scanReal.next();

			negative_exceptions.push_back(real);
		}

		for(unsigned i=0; i<allCommands.size(); i++)
		{
			Scanner scanPrefix(allCommands[i]->id());
			scanPrefix.useSeparators(".");

			if(scanPrefix.next() == prefix)
			{
				string smallName = scanPrefix.next();

				if(in(smallName, negative_exceptions)) // jump negative exceptions in modules
					continue;

				if(moduleExists(smallName, allCommands))
				{
					// WARNING!
					cout << "system.use module warning: couldn't rename '" << allCommands[i]->id() << "' because module '" << smallName << "' is already registered!" << endl;
					continue;
				}

				/*
				stringstream ss;
				ss << allCommands[i]->id() << " " << smallName;


				if(!Command<R, ADS, DS>::run_module("command.rename", allCommands, allFunctions, factory, dictionary, ldictionary, ss.str()))
				{
					cout << "system.use module error: failed to do a command.rename with parameters '" << ss.str() << "'" << endl;
					return false;
				}
				*/

				allCommands[i]->handles.push_back(smallName);
			}
		}

		for(unsigned i=0; i<allFunctions.size(); i++)
		{
			Scanner scanPrefix(allFunctions[i]->id());
			scanPrefix.useSeparators(".");

			if(scanPrefix.next() == prefix)
			{
				string smallName = scanPrefix.next();

				if(in(smallName, negative_exceptions)) // jump negative exceptions in functions
					continue;

				if(functionExists(smallName, allFunctions))
				{
					// WARNING!
					cout << "system.use module warning: couldn't rename '" << allFunctions[i]->id() << "' because function '" << smallName << "' is already registered!" << endl;
					continue;
				}

				/*
				stringstream ss;
				ss << allFunctions[i]->id() << " " << smallName;

				if(!Command<R, ADS, DS>::run_module("function.rename", allCommands, allFunctions, factory, dictionary, ldictionary, ss.str()))
				{
					cout << "system.use module error: failed to do a function.rename with parameters '" << ss.str() << "'" << endl;
					return false;
				}
				*/

				allFunctions[i]->handles.push_back(smallName);
			}
		}


		return true;
	}


	// disable preprocess, only need module prefix
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		// disable preprocess!!
		return new string(input);
	}

};

}

#endif /* OPTFRAME_SYSTEM_USE_MODULE_HPP_ */
