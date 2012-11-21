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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ForEachModule: public OptFrameModule<R, ADS, M>
{
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

	bool exec_command(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, ADS, M>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string rest = m->preprocess(allFunctions, dictionary, scanner.rest());
		return m->run(all_modules, allFunctions, factory, dictionary, rest);
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
		return "for_each $var list_of_values list_of_commands";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string var = scanner.next();

		if (var[0] != '$')
		{
			cout << "Missing operator $ in variable: " << var << endl;
			return false;
		}

		vector<string>* pvalues = OptFrameList::readList(scanner);
		vector<string>  values;
		if(pvalues)
		{
			values = vector<string>(*pvalues);
			delete pvalues;
		}
		else
			return false;

		vector<string>* pcommands = OptFrameList::readList(scanner);
		vector<string>  commands;
		if(pcommands)
		{
			commands = vector<string>(*pcommands);
			delete pcommands;
		}
		else
			return false;

		for (unsigned int v = 0; v < values.size(); v++)
		{
			for (unsigned int c = 0; c < commands.size(); c++)
			{
				string command = var_preprocess(var, values.at(v), commands.at(c));

				if(command.at(0)=='%') // first line comment
					command = "";

				if (command != "")
					if(!exec_command(all_modules, allFunctions, factory, dictionary, command))
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
	virtual string preprocess(vector<OptFrameFunction*>& allFunctions, map<string, string>* dictionary, string input)
	{
		Scanner scanner(input);

		// should skip the removal of comments '%' and variable definitions, otherwise it destroys the commands!

		string input2 = "";

		if(!scanner.hasNext())
			return input2;

		string variable   = scanner.next();
		string discarded1 = scanner.getDiscarded();
		input2.append(discarded1);
		input2.append(variable);

		if(!scanner.hasNext())
			return input2;

		string possibleDefinition = scanner.next();
		string discarded2         = scanner.getDiscarded();

		if(dictionary->count(possibleDefinition) == 0) // Not found in dictionary
		{
			input2.append(discarded2);
			input2.append(possibleDefinition); // EXPLICIT LIST
		}
		else // definition found in dictionary
		{
			string found = dictionary->find(possibleDefinition)->second;

			input2.append(discarded2);
			input2.append(found);
		}

		input2.append(scanner.rest());

		return input2;
	}
};

#endif /* FOREACHMODULE_HPP_ */
