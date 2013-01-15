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

#ifndef OPTFRAME_FOR_MODULE_HPP_
#define OPTFRAME_FOR_MODULE_HPP_

#include<string>

#include "../OptFrameModule.hpp"

#include "SystemRunModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ForModule: public OptFrameModule<R, ADS, M>
{
private:
	OptFrameModule<R, ADS, M>* getModule(vector<OptFrameModule<R, ADS, M>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	bool exec_command(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, ADS, M>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string* rest = m->preprocess(allFunctions, dictionary, ldictionary, scanner.rest());

		if(!rest)
			return NULL;

		//cout << "FOR COMMAND: '" << module << "' input: '" << *rest << "'"<< endl;
		bool b = m->run(all_modules, allFunctions, factory, dictionary, ldictionary, *rest);

		delete rest;

		return b;
	}

public:

	virtual ~ForModule()
	{
	}

	string id()
	{
		return "for";
	}

	string usage()
	{
		return "for var [ = value1 to value2 | = value2 downto value1 ] block_of_commands";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);
		//cout << "for run: '" << input << "'" << endl;

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string var = scanner.next();

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		scanner.next(); // '='

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		int value1 = scanner.nextInt();

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string option = scanner.next(); // 'to' or 'downto'

		if((option != "to") && (option != "downto"))
		{
			cout << "for module: unknown option '" << option << "'! Should be 'to' or 'downto'." << endl;
			return false;
		}

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		int value2 = scanner.nextInt();

		vector<string>* pcommands = OptFrameList::readBlock(scanner);
		vector<string>  commands;
		if(pcommands)
		{
			commands = vector<string>(*pcommands);
			delete pcommands;
		}
		else
			return false;

		// check if all the text was used!
		if(!OptFrameModule<R, ADS, M>::testUnused(id(), scanner))
			return false;

		if(option == "to")
			for (int v = value1; v <= value2; v++)
			{
				stringstream ss;
				ss << var << " " << v;
				if(!OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str()))
					return false;

				if(!OptFrameModule<R, ADS, M>::run_module("system.run", all_modules, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(commands)))
				{
					cout << "for module: (TO) error in command with value='" << v << "'!" << endl;
					return false;
				}
				else
					return true;
			}
		else // downto
			for (int v = value1; v >= value2; v--)
			{
				stringstream ss;
				ss << var << " " << v;
				if(!OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str()))
					return false;

				if(!OptFrameModule<R, ADS, M>::run_module("system.run", all_modules, allFunctions, factory, dictionary, ldictionary, OptFrameList::blockToString(commands)))
				{
					cout << "for module: (DOWNTO) error in command with value='" << v << "'!" << endl;
					return false;
				}
				else
					return true;
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

		string equals       = scanner.next();
		string discarded_eq = scanner.getDiscarded();
		input2.append(discarded_eq);
		input2.append(equals);

		if(!scanner.hasNext())
			return new string(input2);

		string possible_def1  = scanner.next();
		string discarded_def1 = scanner.getDiscarded();

		if(dictionary.count(possible_def1) == 0) // Not found in dictionary
		{
			input2.append(discarded_def1);
			input2.append(possible_def1);
		}
		else // definition found in dictionary
		{
			string found = dictionary.find(possible_def1)->second;

			input2.append(discarded_def1);
			input2.append(found);
		}

		if(!scanner.hasNext())
			return new string(input2);

		string to_downto    = scanner.next();
		string discarded_td = scanner.getDiscarded();
		input2.append(discarded_td);
		input2.append(to_downto);

		if(!scanner.hasNext())
			return new string(input2);

		string possible_def2  = scanner.next();
		string discarded_def2 = scanner.getDiscarded();

		if(dictionary.count(possible_def2) == 0) // Not found in dictionary
		{
			input2.append(discarded_def2);
			input2.append(possible_def2);
		}
		else // definition found in dictionary
		{
			string found = dictionary.find(possible_def2)->second;

			input2.append(discarded_def2);
			input2.append(found);
		}

		if(!scanner.hasNext())
			return new string(input2);

		input2.append(scanner.rest());

		return new string(input2);
	}
};

#endif /* OPTFRAME_FOR_MODULE_HPP_ */
