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

#ifndef OPTFRAME_WHILE_MODULE_HPP_
#define OPTFRAME_WHILE_MODULE_HPP_

#include <string>

#include "../Command.hpp"

#include "SystemRunCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class WhileCommand: public Command<R, ADS, DS>
{
public:

	virtual ~WhileCommand()
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

	bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "INPUT: '" << input << "'" << endl;

		stringstream boolean_expr;
		unsigned j = 0;
		for(unsigned i=0; i<input.size(); i++)
			if(input.at(i)!='{')
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
		vector<string>* p_commands = OptFrameList::readBlock(scanner);
		if(p_commands)
		{
			commands = vector<string>(*p_commands);
			delete p_commands;
		}
		else
			return false;

		// check if all the text was used!
		if(!Command<R, ADS, DS>::testUnused(id(), scanner))
			return false;

		string* scond1 = Command<R, ADS, DS>::defaultPreprocess(allFunctions, factory, dictionary, ldictionary, boolean_expr.str());

		if(!scond1)
			return nullptr;

		string scondition = *scond1;

		delete scond1;

		while(parseBool(scondition))
		{
			if(!Command<R, ADS, DS>::run_module("system.run", allCommands, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(commands)))
			{
				cout << "while command: error in command!" << endl;
				return false;
			}

			string* scond = Command<R, ADS, DS>::defaultPreprocess(allFunctions, factory, dictionary, ldictionary, boolean_expr.str());

			if(!scond)
				return false;

			scondition = *scond;
			delete scond;
		}

		return true;
	}


	// should preprocess only until list of commands
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		// disable preprocess!!
		return new string(input);
	}
};

}

#endif /* OPTFRAME_WHILE_MODULE_HPP_ */
