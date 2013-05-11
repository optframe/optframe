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

#ifndef FOREACHMODULE_HPP_
#define FOREACHMODULE_HPP_

#include<string>

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ForEachModule: public OptFrameModule<R, ADS, DS>
{
private:
	OptFrameModule<R, ADS, DS>* getModule(vector<OptFrameModule<R, ADS, DS>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (modules[i]->canHandle(module, "")) // TODO: why?
				return modules[i];
		return NULL;
	}

	bool exec_command(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, ADS, DS>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string* rest = m->preprocess(allFunctions, dictionary, ldictionary, scanner.rest());

		if(!rest)
			return NULL;

		//cout << "FOR_EACH COMMAND: '" << module << "' input: '" << *rest << "'"<< endl;
		bool b = m->run(all_modules, allFunctions, factory, dictionary, ldictionary, *rest, module);

		delete rest;

		return b;
	}

public:

	virtual ~ForEachModule()
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

	bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
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

		vector<string>* pvalues = OptFrameList::readList(ldictionary, scanner);
		vector<string>  values;
		if(pvalues)
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
		vector<string>  commands;
		if(pcommands)
		{
			commands = vector<string>(*pcommands);
			delete pcommands;
		}
		else
		{
			cout << "for_each error: couldn't read block of commands!" << endl;
			return false;
		}

		if(!OptFrameModule<R, ADS, DS>::testUnused(id(), scanner))
			return false;

		for (unsigned int v = 0; v < values.size(); v++)
		{
			for (unsigned int c = 0; c < commands.size(); c++)
			{
				string command = commands[c];
				if(command == "")
				{
					cout << "for_each module: empty command (possible extra semicolon)!" << endl;
					return false;
				}

				string defCommand = "system.silent_define ";
				defCommand.append(var);
				defCommand.append(" ");
				defCommand.append(values[v]);

				if(!exec_command(all_modules, allFunctions, factory, dictionary, ldictionary, defCommand))
				{
					cout << "for_each error: failed to define variable '" << var << "' to value '" << values[v] << "'" << endl;
					return false;
				}

				if(command.at(0)=='%') // first line comment
					command = "";

				//cout << "FOR_EACH COMMAND: '" << command << "'" << endl;

				if (command != "")
					if(!exec_command(all_modules, allFunctions, factory, dictionary, ldictionary, command))
					{
						if(commands.at(c)=="")
							cout << "for_each module: empty command! (perhaps an extra comma in list?)" << endl;
						else
							cout << "for_each module: error in command '" << commands.at(c) << "'" << endl;

						return false;
					}

			}
		}

		return true;
	}

	// should preprocess only until list of commands
	virtual string* preprocess(vector<OptFrameFunction*>& allFunctions, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		// should skip the removal of comments '%' and variable definitions, otherwise it destroys the commands!

		string input2 = "";

		if(!scanner.hasNext())
			return new string(input2);

		string variable   = scanner.next();
		string discarded1 = scanner.getDiscarded();
		input2.append(discarded1);
		input2.append(variable);

		if(!scanner.hasNext())
			return new string(input2);

		string possibleDefinition = scanner.next();
		string discarded2         = scanner.getDiscarded();

		if(dictionary.count(possibleDefinition) == 0) // Not found in dictionary
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

#endif /* FOREACHMODULE_HPP_ */
