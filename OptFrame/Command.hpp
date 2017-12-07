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

/*
 * Command.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAMEMODULE_HPP_
#define OPTFRAMEMODULE_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "Scanner++/Scanner.h"

#include "PreprocessFunction.hpp"

#include "HeuristicFactory.hpp"

#include <algorithm>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Command
{
protected:
	static int precision; // for floats and doubles

	bool run_module(string mod, vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& f, map<string,string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		for(unsigned int i=0;i<allCommands.size();i++)
			if(allCommands[i]->canHandle(mod, input))
			{
				// TODO: NO PREPROCESSING?? WHY?
				return allCommands[i]->run(allCommands, allFunctions, f, dictionary, ldictionary, input, mod);
			}
		cout << "Command '"<<mod<<"' not found." << endl;
		return false;
	}

public:

	vector<string> handles;

	virtual ~Command()
	{
	}

	virtual bool canHandle(string module_name, string command_body)
	{
		if (id() == module_name)
			return true;

		for (unsigned i = 0; i < handles.size(); i++)
			if (handles[i] == module_name)
				return true;

		return false;
	}

	virtual string id() = 0;

	// default: hashing by id()
    virtual unsigned long hash()
	{
    	const char *str = id().c_str();

		unsigned long h = 5381;
		int c;

		while ((c = *str++))
			h = ((h << 5) + h) + c; /*option: hash * 33 + c */

		return h;
	}

	virtual string usage() = 0;

	static bool validVariableName(string var)
	{
		if(var.length()==0)
		{
			//cout << "Command::validVariableName error: empty variable!" << endl;
			return false;
		}

		bool number = true;
		try
		{
			Scanner::parseDouble(var);
		}
		catch(ConversionError& e)
		{
			number = false;
		}

		if(number)
		{
			//cout << "Command::validVariableName error: cannot define a number! ('" << var << "')" << endl;
			//cout << "Did you forget a command 'undefine variable' before this module?" << endl;
			return false;
		}

		Scanner scanType(var);
		scanType.useSeparators(":");
		string optframe = scanType.next();
		if(optframe == "OptFrame")
		{
			//cout << "Command defineText: cannot define an OptFrame type! ('" << var << "')" << endl;
			//cout << "Did you forget a command 'undefine variable' before this module?" << endl;
			return false;
		}

		return true;
	}

	static bool isInDictionary(string var, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		return (dictionary.count(var) > 0) || (ldictionary.count(var) > 0);
	}

	// do: new_var => content of target_var
	static bool pointVars(string new_var, string target_var, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		if (dictionary.count(target_var) > 0)
		{
			return defineText(new_var, dictionary.find(target_var)->second, dictionary);
		}
		else if (ldictionary.count(target_var) > 0)
		{
			return defineList(new_var, ldictionary.find(target_var)->second, ldictionary);
		}
		else
		{
			cout << "Command::pointVars error: variable '" << target_var << "' not found!" << endl;
			return false;
		}
	}

	static string solveTextVar(string variable, map<string, string>& dictionary)
	{
		Scanner scanDef(variable);
		scanDef.useSeparators("$");
		if(!scanDef.hasNext())
			return "";

		string var_name = scanDef.next();

		//cout << "pure var_name = " << var_name << endl;

		if(dictionary.count(var_name) == 0)
		{
			cout << "Command::solveReference error: variable '" << variable << "' not defined!" << endl;
			return "";
		}

		return dictionary.find(var_name)->second;
	}

	static bool define(string variable, string value, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		string trimValue = Scanner::trim(value);

		Scanner scanner(trimValue);

		if ((trimValue == "") || (!scanner.hasNext()))
			return false;

		string next = scanner.next();

		// is list
		if ((next[0] == '[') || (ldictionary.count(trimValue) > 0))
		{
			if(dictionary.count(variable) > 0)
			{
				cout << "Command::define error: variable '" << variable << "' already defined in text dictionary!" << endl;
				return false;
			}

			vector<string>* p_list = OptFrameList::readList(ldictionary, value);
			if (p_list)
			{
				vector<string> list(*p_list);
				delete p_list;

				return defineList(variable, list, ldictionary);
			}

			return false;
		}

		// is text
		return defineText(variable, value, dictionary);
	}


	static bool defineText(string definition, string value, map<string,string>& dictionary)
	{
		// will not need this check after better implementation of dictionary with '$' before definitions
		// TODO

		if(!validVariableName(definition))
		{
			cout << "Command::defineText error: not a variable name '" << definition << "'" << endl;
			return false;
		}

		if(definition[0]=='$')
		{
			// TODO: WHY?
			//cout << "defineText error: trying to define variable with dollar '" << definition << "'" << endl;
			//return false;
			string newVar = solveTextVar(definition, dictionary);
			//cout << "newVar='" << newVar << "'" << endl;
			if(newVar=="")
			{
				cout << "defineText error: variable '" << definition << "' is empty!" << endl;
				return false;
			}

			return defineText(newVar, value, dictionary);
		}

		Scanner scanner(value);

		while(scanner.hasNext())
		{
			string next_word = scanner.next();
			if(definition==next_word)
			{
				cout << "defineText: recursive definitions are not allowed! (define: '" << definition << "' as '" << next_word << "')" << endl;
				cout << "Did you forget a command 'undefine variable' before this module?" << endl;
				return false;
			}
		}

		dictionary[definition] = scanner.trim(value);

		return true;
	}

	static bool defineList(string definition, vector<string>& list, map< string,vector<string> >& ldictionary)
	{
		if(!validVariableName(definition))
		{
			cout << "Command::defineList error: not a variable name '" << definition << "'" << endl;
			return false;
		}

		if(definition[0]=='$')
		{
			//TODO: WHY?
			cout << "defineList error: trying to define variable with dollar '" << definition << "' (can't use references for lists)" << endl;
				return false;

			///Scanner scanDef(definition);
			///scanDef.useSeparators("$");
			///return defineList(scanDef.next(), list, ldictionary);
		}

		ldictionary[definition] = list;

		return true;
	}

	static void undefineText(string definition, map<string,string>& dictionary)
	{
		dictionary.erase(definition);
	}

	static void undefineList(string definition, map< string,vector<string> >& ldictionary)
	{
		ldictionary.erase(definition);
	}

	static void undefine(string definition, map<string,string>& dictionary, map< string,vector<string> >& ldictionary)
	{
		undefineText(definition,  dictionary);
		undefineList(definition, ldictionary);
	}


	bool testUnused(string id, Scanner& scanner)
	{
		if(scanner.hasNext())
		{
			cout << "Error in module '" << id << "': unused text '" << scanner.rest() << "'" << endl;
			return false;
		}

		return true;
	}

	virtual bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input, string module_name)
	{
		return run(allCommands, allFunctions, factory, dictionary, ldictionary, input);
	}

	virtual bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input) = 0;

	virtual string* preprocess(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string,string>& dictionary, const map< string,vector<string> >& ldictionary, string input) = 0;

	//{
	//	return defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	//}

	static string* solveVars(const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input, string only_var="")
	{
		//cout << "SOLVEVARS WITH '" << input << "' and only_var='" << only_var<< "'" << endl;
		string result = "";

		bool inVar = false;
		string var = "";

		for (unsigned i = 0; i < input.length(); i++)
		{
			// build var
			if((!inVar) && (var != ""))
			{
				// for variables with brackets {}
				Scanner scanner(var);
				scanner.useSeparators("{}");
				string var2 = scanner.next();

				if ((only_var != "") && (var2 != only_var)) // do not solve now!
				{
					result += '$';
					result.append(var);
					var = "";
					i--;
					continue;
				}

				var = var2;

				if(var == "")
				{
					cout << "solveVars error: empty variable ${}!" << endl;
					return nullptr;
				}

				if (dictionary.count(var) != 0)
					result.append(dictionary.find(var)->second);
				else if (ldictionary.count(var) != 0)
					result.append(OptFrameList::listToString(ldictionary.find(var)->second));
				else
				{
					cout << "Error: variable '$" << var << "' not defined in any dictionary!" << endl;
					return nullptr; // no variable in dictionary!
				}

				var = "";
			}

			if (inVar)
			{
				if ((input[i] != ' ') && (input[i] != '\t') && (input[i] != '\n') && (input[i] != '$') && (input[i] != '}'))
				{
					var += input[i];
					continue;
				}
				else
				{
					inVar = false;

					// close variable
					if (input[i] == '}')
					{
						var += '}';
						continue;
					}
					else
					{
						i--;
						continue;
					}
				}
			}

			// avoid double $$
			if((!inVar) && (input[i] == '$') && (i + 1 < input.length()) && (input[i + 1] == '$'))
			{
				if (only_var == "") // can solve
				{
					result += '$';
					i++;
				}
				else // do not solve now
				{
					result.append("$$");
					i += 2;
				}
				continue;
			}

			// capture var
			if ((!inVar) && (input[i] == '$'))
				inVar = true;

			// ordinary char
			if (!inVar)
				result += input[i];
		}

		// build last var
		if (var != "")
		{
			// for variables with brackets {}
			Scanner scanner(var);
			scanner.useSeparators("{}");
			string var2 = scanner.next();

			if ((only_var != "") && (var2 != only_var)) // do not solve now!
			{
				result += '$';
				result.append(var);
				return new string(result);
			}

			var = var2;

			if (var == "")
			{
				cout << "solveVars error: empty variable ${}!" << endl;
				return nullptr;
			}

			if (dictionary.count(var) != 0)
				result.append(dictionary.find(var)->second);
			else if (ldictionary.count(var) != 0)
				result.append(OptFrameList::listToString(ldictionary.find(var)->second));
			else
			{
				cout << "Error: variable '$" << var << "' not defined in any dictionary!" << endl;
				return nullptr; // no variable in dictionary!
			}
		}


		return new string(result);
	}

	static string* defaultPreprocess(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string,string>& dictionary, const map< string,vector<string> >& ldictionary, string input)
	{
		//stringstream input_func; // add spaces before and after '(', ')', '[', ']', '{', '}', ';' and ','
		string input_func = ""; // add spaces before and after '(', ')'
		for(unsigned i=0; i<input.size(); i++)
		{
			//if( (input.at(i)=='(') || (input.at(i)==')') || (input.at(i)=='[') || (input.at(i)==']') || (input.at(i)==',') || (input.at(i)=='{') || (input.at(i)=='}') || (input.at(i)==';') )
			if( (input.at(i)=='(') || (input.at(i)==')') )
			{
				input_func += ' ';
				input_func += input.at(i);
				input_func += ' ';
			}
			else
				input_func += input.at(i);
		}

		// ===============
		// solve variables
		// ===============
		string* p_input = solveVars(dictionary, ldictionary, input_func);
		if (!p_input)
			return nullptr;
		else
		{
			input = *p_input;
			delete p_input;
		}


		Scanner scanFunc(input);

		// Second, use the dictionary

		string input5 = "";

		string last       = scanFunc.next();
		string ldiscarded = scanFunc.getDiscarded();

		while(scanFunc.hasNext())
		{
			string current    = scanFunc.next();
			string cdiscarded = scanFunc.getDiscarded();

			if( (current == "(") && PreprocessFunction<R,ADS,DS>::functionExists(last, allFunctions) ) // FUNCTION
			{
				pair<string, string>* p = PreprocessFunction<R,ADS,DS>::run_function(last, allFunctions, hf, dictionary, ldictionary, scanFunc.rest());

				if(p)
				{
					input5.append(" ");
					input5.append(p->first);
					input5.append(" ");

					scanFunc = Scanner(p->second);

					delete p;

					if(!scanFunc.hasNext())
					{
						last = "";
						ldiscarded = "";
						break;
					}
					else
					{
						last       = scanFunc.next();
						ldiscarded = scanFunc.getDiscarded();
						continue;
					}
				}
				else
				{
					cout << "module preprocessing error in function '" << last << "'" << endl;
					return nullptr; // error in valid function!
				}
			}
			else
			{
				input5.append(ldiscarded);
				input5.append(last);
			}

			// update last
			ldiscarded = cdiscarded;
			last       = current;
		}

		input5.append(ldiscarded);
		input5.append(last);

		string input6 = Scanner::trim(input5);

		return new string(input6);
	}
};

template<class R, class ADS, class DS>
int Command<R,ADS,DS>::precision=2;

}

#endif /* OPTFRAMEMODULE_HPP_ */
