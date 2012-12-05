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
			return command; // no possible variable!

		string new_command = "";
		string rest = "";
		int dollar_pos = -1;

		for (unsigned int i = 0; i < command.length(); i++)
		{
			if (command[i] == '$')
			{
				dollar_pos = i;//first dollar position
				break;
			}
		}

		if (dollar_pos == -1) //dollar not found
			new_command = command;
		else
		{
			if (command[dollar_pos + 1] == '{') //var with brackets
			{
				//verifying if have close bracket for this var
				if (command[dollar_pos + var.length() + 1] == '}')
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
			else// variable without brackets
			{
				if (isalnum(command[dollar_pos + var.length()]) || (command[dollar_pos + var.length()] == '_'))// don't have the same length, can be another variable
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
		}

		return new_command;

	}

	bool exec_command(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, ADS, M>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string rest = m->preprocess(allFunctions, dictionary, scanner.rest());
		m->run(all_modules, allFunctions, factory, dictionary, rest);

		return true;
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

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, string input)
	{
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

		for (unsigned int c = 0; c < commands.size(); c++)
		{
			string command = commands[c];

			for (unsigned int v = 0; v < values.size(); v++)
				command = var_preprocess(parameters[v], values[v], command);

			if (!exec_command(all_modules, allFunctions, factory, dictionary, command))
			{
				cout << "Error in command: " << command << endl;
				return false;
			}
		}

		return true;
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class CreateModule: public OptFrameModule<R, ADS, M>
{
private:

	OptFrameModule<R, ADS, M>* getModule(vector<OptFrameModule<R, ADS, M>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

public:

	virtual ~CreateModule()
	{
	}

	string id()
	{
		return "create_module";
	}

	string usage()
	{
		return "create_module name list_of_$parameters list_of_commands";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, string input)
	{
		Scanner scanner(input);
		//cout << "create_module run: '" << input << "'" << endl;

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string name = scanner.next();

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>* plist1 = OptFrameList::readList(scanner);
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

		vector<string>* plist = OptFrameList::readList(scanner);
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

	virtual string preprocess(vector<OptFrameFunction*>& allFunctions, map<string, string>& dictionary, string input)
	{
		// disable preprocess!!
		return input;
	}
};

#endif /* CREATEMODULE_HPP_ */
