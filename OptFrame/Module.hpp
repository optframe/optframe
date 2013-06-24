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
 * Module.hpp
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
class Module
{
protected:
	static int precision; // for floats and doubles

	bool run_module(string mod, vector<Module<R, ADS, DS>*>& allModules, vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& f, map<string,string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		for(unsigned int i=0;i<allModules.size();i++)
			if(allModules[i]->canHandle(mod, input))
			{
				// TODO: NO PREPROCESSING?? WHY?
				return allModules[i]->run(allModules, allFunctions, f, dictionary, ldictionary, input, mod);
			}
		cout << "Module '"<<mod<<"' not found." << endl;
		return false;
	}

public:

	vector<string> handles;

	virtual ~Module()
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

	static bool defineText(string definition, string value, map<string,string>& dictionary)
	{
		// will not need this check after better implementation of dictionary with '$' before definitions
		// TODO

		if(definition.length()==0)
		{
			cout << "defineText error: empty variable!" << endl;
			return false;
		}

		if(definition[0]=='$')
		{
			cout << "defineText error: trying to define variable with dollar '" << definition << "'" << endl;
			return false;
		}


		bool number = true;
		try
		{
			Scanner::parseDouble(definition);
		}
		catch(ConversionError& e)
		{
			number = false;
		}
		if(number)
		{
			cout << "Module defineText: cannot define a number! ('" << definition << "')" << endl;
			cout << "Did you forget a command 'undefine variable' before this module?" << endl;
			return false;
		}

		Scanner scanType(definition);
		scanType.useSeparators(":");
		string optframe = scanType.next();
		if(optframe == "OptFrame")
		{
			cout << "Module defineText: cannot define an OptFrame type! ('" << definition << "')" << endl;
			cout << "Did you forget a command 'undefine variable' before this module?" << endl;
			return false;
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
		if(definition.length()==0)
		{
			cout << "defineList error: empty variable!" << endl;
			return false;
		}

		if(definition[0]=='$')
		{
			cout << "defineList error: trying to define variable with dollar '" << definition << "'" << endl;
			return false;
		}

		bool number = true;
		try
		{
			Scanner::parseDouble(definition);
		}
		catch(ConversionError& e)
		{
			number = false;
		}
		if(number)
		{
			cout << "Module defineList: cannot define a number! ('" << definition << "')" << endl;
			cout << "Did you forget a command 'undefine variable' before this module?" << endl;
			return false;
		}

		Scanner scanType(definition);
		scanType.useSeparators(":");
		string optframe = scanType.next();
		if(optframe == "OptFrame")
		{
			cout << "Module defineList: cannot define an OptFrame type! ('" << definition << "')" << endl;
			cout << "Did you forget a command 'undefine variable' before this module?" << endl;
			return false;
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

	virtual bool run(vector<Module<R, ADS, DS>*>& allModules, vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input, string module_name)
	{
		return run(allModules, allFunctions, factory, dictionary, ldictionary, input);
	}

	virtual bool run(vector<Module<R, ADS, DS>*>& allModules, vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input) = 0;

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
					return NULL;
				}

				if (dictionary.count(var) != 0)
					result.append(dictionary.find(var)->second);
				else if (ldictionary.count(var) != 0)
					result.append(OptFrameList::listToString(ldictionary.find(var)->second));
				else
				{
					cout << "Error: variable '$" << var << "' not defined in any dictionary!" << endl;
					return NULL; // no variable in dictionary!
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
				return NULL;
			}

			if (dictionary.count(var) != 0)
				result.append(dictionary.find(var)->second);
			else if (ldictionary.count(var) != 0)
				result.append(OptFrameList::listToString(ldictionary.find(var)->second));
			else
			{
				cout << "Error: variable '$" << var << "' not defined in any dictionary!" << endl;
				return NULL; // no variable in dictionary!
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
			return NULL;
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
					return NULL; // error in valid function!
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
int Module<R,ADS,DS>::precision=2;

}

#endif /* OPTFRAMEMODULE_HPP_ */
