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

#ifndef CREATEMODULE_HPP_
#define CREATEMODULE_HPP_

#include<string>

#include "../Module.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class GeneralModule: public Module<R, ADS, DS>
{
	string name;
	vector<string> parameters;
	vector<string> commands;

public:
	GeneralModule(string _name, vector<string>& _parameters, vector<string>& _commands)
	{
		name = _name;
		parameters = _parameters;
		commands = _commands;
		//cout << "GENERAL MODULE(" << _name << "," << _parameters << "," << _commands << ")" << endl;
	}

private:
	Module<R, ADS, DS>* getModule(vector<Module<R, ADS, DS>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (modules[i]->canHandle(module, "")) // TODO: fix
				return modules[i];
		return NULL;
	}


	bool exec_command(vector<Module<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		Module<R, ADS, DS>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string* rest = m->preprocess(allFunctions, factory, dictionary, ldictionary, scanner.rest());
		if(!rest)
			return false;

		bool b = m->run(all_modules, allFunctions, factory, dictionary, ldictionary, *rest);

		delete rest;

		return b;
	}

public:

	virtual ~GeneralModule()
	{
	}

	string id()
	{
		return name;
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

	bool run(vector<Module<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		// CHECK IF EXPLICIT LIST IS PASSED AS PARAMETER (CAN'T DO THIS!!!)
		for(unsigned i=0; i<input.size(); i++)
			if(input.at(i)=='[')
			{
				cout << "dynamic_module '" << id() << "' error: can't have explicit list as parameter! use 'silent_define_list' before calling this!" << endl;
				return false;
			}

		Scanner scanner(input);

		//cout << "general module '" << id() << "' (created) run: '" << input << "'" << endl;

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector < string > values;

		for (unsigned int i = 0; i < parameters.size(); i++)
			if (!scanner.hasNext())
			{
				cout << "Usage: " << usage() << endl;
				return false;
			}
			else
				values.push_back(scanner.next());


		for (unsigned v = 0; v < values.size(); v++)
		{
			//cout << "CREATED MODULE " << id() << " DEFINING: '" << parameters[v] << "' as '" << values[v] << "'" << endl;
			if (!Module<R, ADS, DS>::defineText(parameters[v], values[v], dictionary))
			{
				cout << "module.create error: failed to define parameter '" << parameters[v] << "' to value '" << values[v] << "'" << endl;
				return false;
			}
		}


		//cout << "MODULE '" << id() << "' (CREATED) VALUES: '" << values << "'" << endl;

		for (unsigned int c = 0; c < commands.size(); c++)
		{
			string command = commands[c];
			//cout << "CREATED MODULE " << id() << " COMMAND: '" << command << "'" << endl;
//			command.append(" "); // TODO: why we need this to find variable in the end?

			//cout << "MODULE '" << id() << "' (CREATED) COMMAND: '" << command << "'" << endl;

			for (unsigned int v = 0; v < values.size(); v++)
			{
				//cout << "CREATED MODULE " << id() << " SOLVING VARIABLE '" << parameters[v] << "'" << endl;
				string* r = Module<R, ADS, DS>::solveVars(dictionary,ldictionary, command, parameters[v]);
				if(!r)
				{
					cout << "General module '" << id() << "' error: couldn't solve variable '" << parameters[v] << "'" << endl;
					return false;
				}
				command = *r;
				//cout << "CREATED MODULE AFTER SOLVE: '" << command << "'" << endl << endl;
				delete r;
			}

			//cout << "CREATED MODULE '" << id() << "' EXEC COMMAND: '" << command << "'" << endl;

			if (!exec_command(all_modules, allFunctions, factory, dictionary, ldictionary, command))
			{
				cout << "dynamic_module '" << id() << "' error in command: '" << command << "'" << endl;
				return false;
			}
		}

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Module<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ModuleCreateModule: public Module<R, ADS, DS>
{
private:

	bool moduleExists(string moduleName, vector<Module<R, ADS, DS>*>& allModules)
	{
		for(unsigned i=0; i<allModules.size(); i++)
			if(allModules[i]->id() == moduleName)
				return true;
		return false;
	}

	Module<R, ADS, DS>* getModule(vector<Module<R, ADS, DS>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

public:

	virtual ~ModuleCreateModule()
	{
	}

	string id()
	{
		return "module.create";
	}

	string usage()
	{
		return "module.create name list_of_$parameters block_of_commands";
	}

	bool run(vector<Module<R, ADS, DS>*>& modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
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
			cout << "module.create module: couldn't create module '" << name << "' because it already exists!" << endl;
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
			delete plist1;
		}
		else
		{
			cout << "module.create error: ill-formed parameter list for new module '" << name << "'!" << endl;
			return false;
		}

		for (unsigned int i = 0; i < parameters.size(); i++)
			if (parameters[i][0] == '$')
			{
				cout << "module.create error: operator $ in variable: '" << parameters[i] << "'" << endl;
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
		{
			cout << "module.create error: ill-formed command block for new module '" << name << "'!" << endl;
			return false;
		}

		Module<R, ADS, DS>* m = getModule(modules, name);

		if (m != NULL)
		{
			cout << "module.create error: module with name '" << name << "' already exists!" << endl;
			return false;
		}
		else
		{
			modules.push_back(new GeneralModule<R, ADS, DS> (name, parameters, commands));
			//cout << "module '" << name << "' loaded." << endl;
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
			return NULL; // no brackets

		string* pbody = Module<R, ADS, DS>::defaultPreprocess(fs, hf, d, ld, body);

		if (!pbody)
			return NULL;

		string ninput = *pbody;

		delete pbody;

		for (int i = end; i < ((int) input.length()); i++)
			ninput += input[i];

		return new string(ninput);
	}
};

}

#endif /* CREATEMODULE_HPP_ */
