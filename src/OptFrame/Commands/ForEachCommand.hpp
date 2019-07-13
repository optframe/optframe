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

#ifndef FOREACHMODULE_HPP_
#define FOREACHMODULE_HPP_

#include<string>

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ForEachCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ForEachCommand()
	{
	}

	string id()
	{
		return "for_each";
	}

	string usage()
	{
		return "for_each variable list_of_values block_of_commands";
	}

	bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);
		//cout << "for_each run: '" << input << "'" << endl;

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string var = scanner.next();

		if (var[0] == '$')
		{
			cout << "for_each error: operator $ in variable '" << var << "'" << endl;
			return false;
		}

		bool initialStatusVar = Command<R, ADS, DS>::isInDictionary(var, dictionary, ldictionary);

		vector<string>* pvalues = OptFrameList::readList(ldictionary, scanner);
		vector<string> values;
		if (pvalues)
		{
			values = vector<string>(*pvalues);
			delete pvalues;
		}
		else
		{
			cout << "for_each error: couldn't read input list!" << endl;
			return false;
		}

		vector<string>* pcommands = OptFrameList::readBlock(scanner);
		vector<string> commands;
		if (pcommands)
		{
			commands = vector<string>(*pcommands);
			delete pcommands;
		}
		else
		{
			cout << "for_each error: couldn't read block of commands!" << endl;
			return false;
		}

		if (!Command<R, ADS, DS>::testUnused(id(), scanner))
			return false;

		for (unsigned int v = 0; v < values.size(); v++)
		{
			Command<R, ADS, DS>::undefine(var, dictionary, ldictionary);

			if (!Command<R, ADS, DS>::define(var, values[v], dictionary, ldictionary))
			{
				cout << "for_each error: failed to define variable '" << var << "' to value '" << values[v] << "'" << endl;
				return false;
			}

			if (!Command<R, ADS, DS>::run_module("system.run", allCommands, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(commands)))
			{
				cout << "for_each command: error in command!" << endl;
				return false;
			}
		}

		if(!initialStatusVar)
			Command<R, ADS, DS>::undefine(var, dictionary, ldictionary);

		return true;
	}

	// should preprocess only until list of commands
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		// should skip the removal of comments '%' and variable definitions, otherwise it destroys the commands!

		string input2 = "";

		if (!scanner.hasNext())
			return new string(input2);

		string variable = scanner.next();
		string discarded1 = scanner.getDiscarded();
		input2.append(discarded1);
		input2.append(variable);

		if (!scanner.hasNext())
			return new string(input2);

		string possibleDefinition = scanner.next();
		string discarded2 = scanner.getDiscarded();

		if (dictionary.count(possibleDefinition) == 0) // Not found in dictionary
		{
			input2.append(discarded2);
			input2.append(possibleDefinition); // EXPLICIT LIST
		}
		else // definition found in dictionary
		{
			string found = dictionary.find(possibleDefinition)->second;

			input2.append(discarded2);
			input2.append(found);
		}

		input2.append(scanner.rest());

		return new string(input2);
	}
};

}

#endif /* FOREACHMODULE_HPP_ */
