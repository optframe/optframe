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

#ifndef OPTFRAME_WHILE_MODULE_HPP_
#define OPTFRAME_WHILE_MODULE_HPP_

#include <string>

#include "../OptFrameModule.hpp"

#include "RunListModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class WhileModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~WhileModule()
	{
	}

	string id()
	{
		return "while";
	}

	string usage()
	{
		return "while boolean list_of_commands";
	}

	string formatBool(bool b)
	{
		if(b)
			return "true";
		else
			return "false";
	}

	bool parseBool(string b)
	{
		return b == "true";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "INPUT: '" << input << "'" << endl;

		stringstream boolean_expr;
		unsigned j = 0;
		for(unsigned i=0; i<input.size(); i++)
			if(input.at(i)!='[')
			{
				boolean_expr << input.at(i);
				j++;
			}
			else
				break;

		//cout << "'" << input.at(j) << "'" << endl; // should be a '['

		//cout << "BOOLEAN EXPRESSION: '" << Scanner::trim(boolean_expr.str()) << "'" << endl;

		stringstream sscommands;
		for(unsigned k = j; k<input.size(); k++)
			sscommands << input.at(k);

		//cout << "COMMANDS: '" << sscommands.str() << "'" << endl;

		Scanner scanner(sscommands.str());

		vector<string> commands;
		vector<string>* p_commands = OptFrameList::readList(ldictionary, scanner);
		if(p_commands)
		{
			commands = vector<string>(*p_commands);
			delete p_commands;
		}
		else
			return false;


		string* scond1 = OptFrameModule<R, ADS, M>::defaultPreprocess(allFunctions, dictionary, ldictionary, boolean_expr.str());

		if(!scond1)
			return NULL;

		string scondition = *scond1;

		delete scond1;

		while(parseBool(scondition))
		{
			if(!OptFrameModule<R, ADS, M>::run_module("system.run_list", allModules, allFunctions, factory, dictionary, ldictionary, OptFrameList::listToString(commands)))
			{
				cout << "while module: error in command!" << endl;
				return false;
			}

			string* scond = OptFrameModule<R, ADS, M>::defaultPreprocess(allFunctions, dictionary, ldictionary, boolean_expr.str());

			if(!scond)
				return false;

			scondition = *scond;
			delete scond;
		}

		return true;
	}

	// should preprocess only until list of commands
	virtual string* preprocess(vector<OptFrameFunction*>&, map<string, string>&, map< string,vector<string> >&, string input)
	{
		// disable preprocess!!
		return new string(input);
	}
};

#endif /* OPTFRAME_WHILE_MODULE_HPP_ */
