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

#ifndef OPTFRAME_SYSTEM_RUN_MODULE_HPP_
#define OPTFRAME_SYSTEM_RUN_MODULE_HPP_

#include<string>

#include "../Command.hpp"


namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemRunCommand: public Command<R, ADS, DS>
{
public:
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

	bool exec_command(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		scanner.useSeparators(" \t\r\n=");

		string module = scanner.next();
		string tail = scanner.rest();
		Command<R, ADS, DS>* m = getCommand(all_modules, module, tail);

		if (m == nullptr)
		{
			cout << "system.run module: command '" << module << "' not found!" << endl;
			return false;
		}

		string* rest = m->preprocess(allFunctions, factory, dictionary, ldictionary, tail);

		if(!rest)
		{
			delete rest;
			return false;
		}

		//cout << "RUN LIST COMMAND: '" << module << "' with '" << *rest << "'" << endl;
		bool b = m->run(all_modules, allFunctions, factory, dictionary, ldictionary, *rest, module);

		delete rest;

		return b;
	}

	virtual ~SystemRunCommand()
	{
	}

	string id()
	{
		return "system.run";
	}

	string usage()
	{
		return "system.run block_of_commands | module_name [dictionary_entry]";
	}

	bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input1)
	{
		string input = Scanner::trim(input1);

		//cout << "system.run command: '" << input << "'" << endl;

		if(input.length()==0)
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		// multiple commands
		if(input.at(0) == '{')
		{
			vector<string>  lcommands;
			vector<string>* p_lcommands = OptFrameList::readBlock(scanner);
			if(p_lcommands)
			{
				lcommands = vector<string>(*p_lcommands);
				delete p_lcommands;
			}
			else
			{
				cout << "system.run command: error reading block of commands!" << endl;
				return false;
			}

			for (unsigned int c = 0; c < lcommands.size(); c++)
			{
				string command = lcommands.at(c);

				if (command != "")
					if (!exec_command(allCommands, allFunctions, factory, dictionary, ldictionary, command))
					{
						if (lcommands.at(c) == "")
							cout << "system.run command: empty command! (perhaps an extra semicolon in block?)" << endl;
						else
							cout << "system.run command: error in command '" << lcommands.at(c) << "'" << endl;

						return false;
					}
			}

			if(!Command<R, ADS, DS>::testUnused(id(), scanner))
				return false;

			return true;
		}
		else // single command
		{
			string module_name = scanner.next();

			stringstream ss;

			if(scanner.hasNext())
			{
				string new_word = scanner.next();
				if(new_word[0]=='$') // variable?
				{
					string* r = Command<R, ADS, DS>::solveVars(dictionary, ldictionary, new_word);
					if(!r)
					{
						cout << "module " << id() << " error: failed to solve variable '" << new_word << "'" << endl;
						return false;
					}

					ss << *r;
					delete r;
				}
				else
					ss << new_word; // simple word
			}

			ss << scanner.rest();

			if(!Command<R, ADS, DS>::run_module(module_name, allCommands, allFunctions, factory, dictionary, ldictionary, ss.str()))
			{
				cout << "system.run command: error in command!" << endl;
				return false;
			}
			else
			{
				if(!Command<R, ADS, DS>::testUnused(id(), scanner))
					return false;

				return true;
			}
		}
	}


	// runs raw module without preprocessing
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return new string(input); // disable pre-processing
	}
};

}

#endif /* OPTFRAME_SYSTEM_RUN_MODULE_HPP_ */
