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

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class GeneralModule: public OptFrameModule<R, ADS, M>
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
	}

private:
	OptFrameModule<R, ADS, M>* getModule(vector<OptFrameModule<R, ADS, M>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	string var_preprocess(string var, string value, string command)
	{
		if(command.length() < var.length())
		{
			//cout << "general module var_preprocess: command='" << command << "' var='" << var << "'" << endl;
			//cout << "general module var_preprocess: no possible variable because command is smaller!" << endl;
			return command; // no possible variable!
		}

		//cout << "general module var_preprocess: command='" << command << "' var='" << var << "' value='" << value << "'" << endl;

		string new_command = "";
		string rest = "";
		int dollar_pos = -1;

		for (unsigned int i = 0; i < command.length(); i++)
			if (command[i] == '$')
			{
				dollar_pos = i;//first dollar position
				break;
			}

		if (dollar_pos == -1) //dollar not found
			return command;

		if(((int)command.size()) == (dollar_pos+1)) // just a dollar
		{
			cout << "general module warning: just a dollar!" << endl;
			return command;
		}

		if ( ((dollar_pos+1) < ((int)command.size())) && (command[dollar_pos + 1] == '{') ) //var with brackets
		{
			//verifying if there is a close bracket for this var
			if ( ((dollar_pos + ((int)var.length()) + 1) < ((int)command.size())) && (command[dollar_pos + var.length() + 1] == '}') )
			{
				//verifying if the found var is the same parameter var
				bool test_var = true;
				for (unsigned int i = 1; i < var.length(); i++)//ignoring the $ in the var (i = 1)
				{
					if (var[i] != command[dollar_pos + 1 + i])
					{
						test_var = false;
						break;
					}
				}
				if (test_var) // correctly variable found
				{
					//replacing variable
					new_command.append(command, 0, dollar_pos); //first part of command without bracket
					new_command.append(value); // value of variable
					rest.append(command.begin() + dollar_pos + var.length() + 2, command.end());//rest of command
					//verifying if have the same var in the rest of command
					rest = var_preprocess(var, value, rest);

					new_command.append(rest);
				}
				else//can be another variable
				{
					//dont replace the variable
					new_command.append(command, 0, dollar_pos + var.length() + 2);

					rest.append(command.begin() + dollar_pos + var.length() + 2, command.end());//rest of command

					//verifying if have the var in the rest of command
					rest = var_preprocess(var, value, rest);

					new_command.append(rest);

				}

			}
			else//can be another variable
			{
				//ignoring the variable found and continue finding for the parameter variable in the rest of command
				new_command.append(command, 0, dollar_pos + 2);
				rest.append(command.begin() + dollar_pos + 2, command.end());

				rest = var_preprocess(var, value, rest);

				new_command.append(rest);
			}
		}
		else // variable without brackets
			if((dollar_pos + ((int)var.length())) < ((int)command.size()))
			{
				if( isalnum(command[dollar_pos + var.length()]) || (command[dollar_pos + var.length()] == '_'))// don't have the same length, can be another variable
				{
					new_command.append(command, 0, dollar_pos + 1);
					//finding for the parameter variable in the rest of command
					rest.append(command.begin() + dollar_pos + 1, command.end());

					rest = var_preprocess(var, value, rest);

					new_command.append(rest);
				}
				else // have the same length
				{
					//verifying if the found var is the same parameter var
					bool test_var = true;
					for (unsigned int i = 1; i < var.length(); i++)//ignoring the $ in the var (i = 1)
					{
						if (var[i] != command[dollar_pos + i])
						{
							test_var = false;
							break;
						}
					}

					if (test_var) // correctly variable found
					{
						//replacing variable
						new_command.append(command, 0, dollar_pos); //first part of command without dollar
						new_command.append(value); // value of variable
						rest.append(command.begin() + dollar_pos + var.length(), command.end());//rest of command
						//verifying if have the same var in the rest of command
						rest = var_preprocess(var, value, rest);

						new_command.append(rest);
					}
					else//can be another variable
					{
						//dont replace the variable
						new_command.append(command, 0, dollar_pos + var.length());

						rest.append(command.begin() + dollar_pos + var.length(), command.end());//rest of command

						//verifying if have the var in the rest of command
						rest = var_preprocess(var, value, rest);

						new_command.append(rest);
					}

				}
			}

		if(new_command == "")
			return command;
		else
			return new_command;
	}

	bool exec_command(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, ADS, M>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string* rest = m->preprocess(allFunctions, dictionary, ldictionary, scanner.rest());
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

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		// CHECK IF EXPLICIT LIST IS PASSED AS PARAMETER (CAN'T DO THIS!!!)
		for(unsigned i=0; i<input.size(); i++)
			if(input.at(i)=='[')
			{
				cout << "module '" << id() << "' (created) error: can't have explicit list as parameter! use 'silent_define_list' before calling this!" << endl;
				return false;
			}

		Scanner scanner(input);

		//cout << "module '" << id() << "' (created) run: '" << input << "'" << endl;

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

		//cout << "MODULE '" << id() << "' (CREATED) VALUES: '" << values << "'" << endl;

		for (unsigned int c = 0; c < commands.size(); c++)
		{
			string command = commands[c];
			command.append(" "); // TODO: why we need this to find variable in the end?

			//cout << "MODULE '" << id() << "' (CREATED) COMMAND: '" << command << "'" << endl;

			for (unsigned int v = 0; v < values.size(); v++)
				command = var_preprocess(parameters[v], values[v], command);

			//cout << "MODULE '" << id() << "' (CREATED) COMMAND (after var_prep): '" << command << "'" << endl;

			if (!exec_command(all_modules, allFunctions, factory, dictionary, ldictionary, command))
			{
				cout << "Module (just created) '" << id() << "' error in command: '" << command << "'" << endl;
				return false;
			}
		}

		return true;
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ModuleCreateModule: public OptFrameModule<R, ADS, M>
{
private:

	bool moduleExists(string moduleName, vector<OptFrameModule<R, ADS, M>*>& allModules)
	{
		for(unsigned i=0; i<allModules.size(); i++)
			if(allModules[i]->id() == moduleName)
				return true;
		return false;
	}

	OptFrameModule<R, ADS, M>* getModule(vector<OptFrameModule<R, ADS, M>*>& modules, string module)
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

	bool run(vector<OptFrameModule<R, ADS, M>*>& modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
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
			return false;

		for (unsigned int i = 0; i < parameters.size(); i++)
			if (parameters[i][0] != '$')
			{
				cout << "Missing operator $ in variable: " << parameters[i] << endl;
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

		OptFrameModule<R, ADS, M>* m = getModule(modules, name);

		if (m != NULL)
		{
			cout << "error: module with name '" << name << "' already exists!" << endl;
			return false;
		}
		else
		{
			modules.push_back(new GeneralModule<R, ADS, M> (name, parameters, commands));
			//cout << "module '" << name << "' loaded." << endl;
			return true;
		}

	}

	virtual string* preprocess(vector<OptFrameFunction*>&, map<string, string>&,  map< string,vector<string> >&, string input)
	{
		// disable preprocess!!
		return new string(input);
	}
};

#endif /* CREATEMODULE_HPP_ */
