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

#ifndef CREATE_RAW_MODULE_HPP_
#define CREATE_RAW_MODULE_HPP_

#include<string>

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RawCommand: public Command<R, ADS, DS>
{
	string name;
	vector<string> parameters;
	vector<string> commands;

public:
	RawCommand(string _name, vector<string>& _parameters, vector<string>& _commands)
	{
		name = _name;
		parameters = _parameters;
		commands = _commands;
		//cout << "RAW MODULE(" << _name << "," << _parameters << "," << _commands << ")" << endl;
	}

private:
	Command<R, ADS, DS>* getCommand(vector<Command<R, ADS, DS>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (modules[i]->canHandle(module, "")) // TODO: fix
				return modules[i];
		return nullptr;
	}


	bool exec_command(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		Command<R, ADS, DS>* m = getCommand(all_modules, module);

		if (m == nullptr)
			return false;

		string* rest = m->preprocess(allFunctions, factory, dictionary, ldictionary, scanner.rest());

		if(!rest)
			return false;

		bool b = m->run(all_modules, allFunctions, factory, dictionary, ldictionary, *rest);

		delete rest;

		return b;
	}

public:

	virtual ~RawCommand()
	{
	}

	string id()
	{
		return name;
	}

	// default: hashing by id()
	virtual unsigned long hash()
	{
		unsigned long h = aux_hash(name);

		for (unsigned p = 0; p < parameters.size(); p++)
			h += aux_hash(parameters[p]);

		for (unsigned k = 0; k < commands.size(); k++)
			h += aux_hash(commands[k]);

		return h;
	}

	// auxiliar hash function
	unsigned long aux_hash(string s)
	{
		const char *str = s.c_str();

		unsigned long h = 5381;
		int c;

		while ((c = *str++))
			h = ((h << 5) + h) + c; /*option: hash * 33 + c */

		return h;
	}


	string usage()
	{
		string u = name;
		u.append(" ");
		for (unsigned int i = 0; i < parameters.size(); i++)
		{
			u.append(parameters[i]);
			u.append(" ");
		}

		return u;
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		string backup_input = input;

		Scanner scanner(input);

		//cout << "raw_module '" << id() << "' run: '" << input << "'" << endl;

		vector < string > values;
		values.push_back(backup_input); // "$_all_params"

		unsigned nsystem_params = values.size();

		for (unsigned int i = 0; i < (parameters.size()-nsystem_params); i++)
			if (!scanner.hasNext())
			{
				cout << "Usage: " << usage() << endl;
				return false;
			}
			else
				values.push_back(scanner.next());


		for (unsigned v = 0; v < values.size(); v++)
		{
			//cout << "RAW MODULE " << id() << " DEFINING: '" << parameters[v] << "' as '" << values[v] << "'" << endl;
			if (!Command<R, ADS, DS>::defineText(parameters[v], values[v], dictionary))
			{
				cout << "command.create_raw error: failed to define parameter '" << parameters[v] << "' to value '" << values[v] << "'" << endl;
				return false;
			}
		}


		//cout << "MODULE '" << id() << "' (CREATED) VALUES: '" << values << "'" << endl;

		for (unsigned int c = 0; c < commands.size(); c++)
		{
			string command = commands[c];
			//cout << "RAW MODULE " << id() << " COMMAND: '" << command << "'" << endl;

			for (unsigned int v = 0; v < values.size(); v++)
			{
				//cout << "RAW MODULE " << id() << " SOLVING VARIABLE '" << parameters[v] << "'" << endl;
				string* r = Command<R, ADS, DS>::solveVars(dictionary,ldictionary, command, parameters[v]);
				if(!r)
				{
					cout << "Raw module '" << id() << "' error: couldn't solve variable '" << parameters[v] << "'" << endl;
					return false;
				}
				command = *r;
				//cout << "RAW MODULE AFTER SOLVE: '" << command << "'" << endl << endl;
				delete r;
			}

			//cout << "RAW MODULE '" << id() << "' EXEC COMMAND: '" << command << "'" << endl;

			if (!exec_command(all_modules, allFunctions, factory, dictionary, ldictionary, command))
			{
				cout << "raw_module '" << id() << "' error in command: " << command << endl;
				return false;
			}
		}

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& fs, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& d, const map<string, vector<string> >& ld, string input)
	{
		// disable preprocess!! NEVER PREPROCESS HERE, IT'S THE ONLY DIFFERENCE FROM MODULE.CREATE!
		return new string(input);
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CommandCreateRawCommand: public Command<R, ADS, DS>
{
private:

	bool moduleExists(string moduleName, vector<Command<R, ADS, DS>*>& allCommands)
	{
		for(unsigned i=0; i<allCommands.size(); i++)
			if(allCommands[i]->id() == moduleName)
				return true;
		return false;
	}

	Command<R, ADS, DS>* getCommand(vector<Command<R, ADS, DS>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return nullptr;
	}

public:

	virtual ~CommandCreateRawCommand()
	{
	}

	string id()
	{
		return "command.create_raw";
	}

	string usage()
	{
		return "command.create_raw name list_of_$parameters block_of_commands";
	}

	bool run(vector<Command<R, ADS, DS>*>& modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);
		//cout << "create_module run: '" << input << "'" << endl;

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string name = scanner.next();

		if(moduleExists(name, modules))
		{
			cout << "command.create_raw command: couldn't create module '" << name << "' because it already exists!" << endl;
			return false;
		}

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>* plist1 = OptFrameList::readList(ldictionary, scanner);
		vector<string>  parameters;
		if(plist1)
		{
			parameters = vector<string>(*plist1);
			vector<string> system_params;
			string name_all_params = "_all_params_"; // + MODULE_NAME
			name_all_params.append(name);
			system_params.push_back(name_all_params);
			parameters.insert(parameters.begin(),system_params.begin(), system_params.end());
			delete plist1;
		}
		else
			return false;


		for (unsigned int i = 0; i < parameters.size(); i++)
			if (parameters[i][0] == '$')
			{
				cout << "command.create_raw error: operator $ in variable: '" << parameters[i] << "'" << endl;
				return false;
			}

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector < string > commands;

		vector<string>* plist = OptFrameList::readBlock(scanner);
		if(plist)
		{
			commands = vector<string>(*plist);
			delete plist;
		}
		else
			return false;

		Command<R, ADS, DS>* m = getCommand(modules, name);

		if (m != nullptr)
		{
			cout << "command.create_raw error: module with name '" << name << "' already exists!" << endl;
			return false;
		}
		else
		{
			modules.push_back(new RawCommand<R, ADS, DS> (name, parameters, commands));
			//cout << "raw_module '" << name << "' loaded." << endl;
			return true;
		}

	}


	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& fs, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& d, const map<string, vector<string> >& ld, string input)
	{
		int end = -1;
		string body = "";
		for (int i = 0; i < ((int) input.length()); i++)
			if (input[i] == '{')
			{
				end = i;
				break;
			}
			else
				body += input[i];

		if (end < 0)
			return nullptr; // no brackets

		string* pbody = Command<R, ADS, DS>::defaultPreprocess(fs, hf, d, ld, body);

		if (!pbody)
			return nullptr;

		string ninput = *pbody;

		delete pbody;

		for (int i = end; i < ((int) input.length()); i++)
			ninput += input[i];

		return new string(ninput);
	}
};

}

#endif /* CREATE_RAW_MODULE_HPP_ */
