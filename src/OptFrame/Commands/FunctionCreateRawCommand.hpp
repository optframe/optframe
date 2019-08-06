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

#ifndef OPTFRAME_FUNCTION_CREATE_RAW_MODULE_HPP_
#define OPTFRAME_FUNCTION_CREATE_RAW_MODULE_HPP_

#include<string>

#include "../Command.hpp"
#include "../PreprocessFunction.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RawFunction: public PreprocessFunction<R,ADS,DS>
{
	string name;
	vector<string> parameters;
	string body;

public:
	RawFunction(string _name, vector<string>& _parameters, string& _body)
	{
		name = _name;
		parameters = _parameters;
		body = _body;
	}

private:

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

		if(((int)command.size()) == (dollar_pos+1)) // just one dollar
		{
			cout << "raw function warning: just one buck!" << endl;
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

public:

	virtual ~RawFunction()
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

	string* run(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary,  const map< string,vector<string> >& ldictionary, string input)
	{
		// CHECK IF EXPLICIT LIST IS PASSED AS PARAMETER (CAN'T DO THIS!!!) TODO: I DONT KNOW WHY =(
		for(unsigned i=0; i<input.size(); i++)
			if(input.at(i)=='[')
			{
				cout << "raw_function '" << id() << "' error: can't have explicit list as parameter, sorry! Please use 'list.silent_define' before calling this!" << endl;
				return nullptr;
			}

		string backup_input = input;

		Scanner scanner(input);

		//cout << "raw_function '" << id() << "' run: '" << input << "'" << endl;

		vector < string > values;
		values.push_back(backup_input); // "$_all_params"

		unsigned nsystem_params = values.size();

		for (unsigned int i = 0; i < (parameters.size()-nsystem_params); i++)
			if (!scanner.hasNext())
			{
				cout << "Usage: " << usage() << endl;
				return nullptr;
			}
			else
				values.push_back(scanner.next());

		//cout << "FUNCTION '" << id() << "' (CREATED) VALUES: '" << values << "'" << endl;

		stringstream input_body; // add spaces before and after '(', ')', '[', ']', '{', '}', ';' and ','
		for(unsigned i=0; i<body.size(); i++)
			if( (body.at(i)=='(') || (body.at(i)==')') || (body.at(i)=='[') || (body.at(i)==']') || (body.at(i)==',') )
				input_body << ' ' << body.at(i) << ' ';
			else
				input_body << body.at(i);

		string command = input_body.str();
		command.append(" "); // TODO: why we need this to find variable in the end?

		//cout << "FUNCTION '" << id() << "' (CREATED) COMMAND: '" << command << "'" << endl;

		for (unsigned int v = 0; v < values.size(); v++)
			command = var_preprocess(parameters[v], values[v], command);

		//cout << "FUNCTION '" << id() << "' (CREATED) BODY (after var_prep): '" << command << "'" << endl;

		Scanner scanBody(Scanner::trim(command));

		if(!scanBody.hasNext())
			return new string("");

		string nameOrValue = scanBody.next();

		if(!scanBody.hasNext())
			return new string(nameOrValue); // it was value!

		string brackets = scanBody.next();

		if((brackets == "(") && PreprocessFunction<R,ADS,DS>::functionExists(nameOrValue,allFunctions)) // found a function!
		{
			pair<string, string>* p = PreprocessFunction<R,ADS,DS>::run_function(nameOrValue, allFunctions, hf, dictionary, ldictionary, scanBody.rest());

			if(p)
			{
				string value = Scanner::trim(p->first);
				string rest  = Scanner::trim(p->second);

				delete p;

				// DO NOT WARN REST!
				//if(rest != "")
				//	cout << "raw_function (" << name << ") warning: rest is '" << rest << "'" << endl;

				return new string(value);
			}
			else
			{
				cout << "preprocessing error in raw_function '" << name << "'" << endl;
				return nullptr; // error in valid function!
			}
		}
		else
		{
			stringstream ss;
			ss << nameOrValue << scanBody.getDiscarded() << brackets;
			ss << scanBody.rest();

			return new string(ss.str());
		}
	}

	// FUNCTIONS ALWAYS NEED DEFAULT PREPROCESSING!!
	/*
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>&, map<string, string>&,  map< string,vector<string> >&, string input)
	{
		// disable preprocess!!
		return new string(input);
	}
	*/
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class FunctionCreateRawCommand: public Command<R, ADS, DS>
{
private:

	bool functionExists(string  functionName, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions)
	{
		for(unsigned i=0; i<allFunctions.size(); i++)
			if(allFunctions[i]->id() == functionName)
				return true;
		return false;
	}

public:

	virtual ~FunctionCreateRawCommand()
	{
	}

	string id()
	{
		return "function.create_raw";
	}

	string usage()
	{
		return "function.create_raw name list_of_$parameters = body_of_function";
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

		if(functionExists(name, allFunctions))
		{
			cout << "function.create_raw command: couldn't create function '" << name << "' because it already exists!" << endl;
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
			string name_all_params = "$_all_params_"; // + FUNCTION_NAME
			name_all_params.append(name);
			system_params.push_back(name_all_params);
			parameters.insert(parameters.begin(),system_params.begin(), system_params.end());
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

		string eq = scanner.next();
		if(eq != "=")
		{
			cout << "function.create_raw error: expected '=' and found '" << eq << endl;
			return false;
		}

		string body = Scanner::trim(scanner.rest());

		allFunctions.push_back(new RawFunction<R, ADS, DS>(name, parameters, body));

		//cout << "raw_function '" << name << "' loaded with parameters '" << parameters << "' and body: '" << body << "'" << endl;

		return true;
	}


	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		// disable preprocess!!
		return new string(input);
	}
};

}

#endif /* OPTFRAME_FUNCTION_CREATE_RAW_MODULE_HPP_ */
