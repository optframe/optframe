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
 * OptFrameModule.hpp
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

#include "OptFrameFunction.hpp"

#include "HeuristicFactory.hpp"

#include <algorithm>

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class OptFrameModule
{
protected:

	bool run_module(string mod, vector<OptFrameModule<R, ADS, M>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& f, map<string,string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		for(unsigned int i=0;i<allModules.size();i++)
			if(mod==allModules[i]->id())
			{
				// TODO: NO PREPROCESSING?? WHY?
				return allModules[i]->run(allModules, allFunctions, f, dictionary, ldictionary, input);
			}
		cout << "Module '"<<mod<<"' not found." << endl;
		return false;
	}


public:

	virtual ~OptFrameModule()
	{
	}

	virtual string id() = 0;
	virtual string usage() = 0;

	bool defineText(string definition, string value, map<string,string>& dictionary)
	{
		// will not need this check after better implementation of dictionary with '$' before definitions

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
			cout << "OptFrameModule defineText: cannot define a number! ('" << definition << "')" << endl;
			return false;
		}

		Scanner scanner(value);

		while(scanner.hasNext())
		{
			string next_word = scanner.next();
			if(definition==next_word)
			{
				cout << "defineText: recursive definitions are not allowed! (define: '" << definition << "' as '" << next_word << "')" << endl;
				return false;
			}
		}

		dictionary[definition] = scanner.trim(value);

		return true;
	}

	bool defineList(string definition, vector<string>& list, map< string,vector<string> >& ldictionary)
	{
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
			cout << "OptFrameModule defineList: cannot define a number! ('" << definition << "')" << endl;
			return false;
		}

		ldictionary[definition] = list;

		return true;
	}

	void undefineText(string definition, map<string,string>& dictionary)
	{
		dictionary.erase(definition);
	}

	void undefineList(string definition, map< string,vector<string> >& ldictionary)
	{
		ldictionary.erase(definition);
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

	virtual bool run(vector<OptFrameModule<R, ADS, M>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input) = 0;

	virtual string* preprocess(vector<OptFrameFunction*>& allFunctions, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		return defaultPreprocess(allFunctions, dictionary, ldictionary, input);
	}

	static string* defaultPreprocess(vector<OptFrameFunction*>& allFunctions, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		stringstream input_func; // add spaces before and after '(', ')', '[', ']', '{', '}', ';' and ','
		for(unsigned i=0; i<input.size(); i++)
			if( (input.at(i)=='(') || (input.at(i)==')') || (input.at(i)=='[') || (input.at(i)==']') || (input.at(i)==',') || (input.at(i)=='{') || (input.at(i)=='}') || (input.at(i)==';') )
				input_func << ' ' << input.at(i) << ' ';
			else
				input_func << input.at(i);

		Scanner scanner(input_func.str());

		// Second, use the dictionary

		string input3 = "";

		while(scanner.hasNext())
		{
			string new_word = scanner.next();
			string unused = scanner.getDiscarded();

			if(dictionary.count(new_word) == 0) // Not found in dictionary!
			{
				input3.append(unused);
				input3.append(new_word);
			}
			else
			{
				string found = dictionary.find(new_word)->second;

				input3.append(unused);
				input3.append(found);
				input3.append(scanner.rest());

				scanner = Scanner(input3);
				input3 = "";
			}
		}

		string input4 = Scanner::trim(input3);

		// Third, locate functions

		Scanner scanFunc(input4);

		if(!scanFunc.hasNext())
			return new string(input4); // no functions

		string input5 = "";

		string last       = scanFunc.next();
		string ldiscarded = scanFunc.getDiscarded();

		while(scanFunc.hasNext())
		{
			string current    = scanFunc.next();
			string cdiscarded = scanFunc.getDiscarded();

			if( (current == "(") && OptFrameFunction::functionExists(last, allFunctions) ) // FUNCTION
			{
				pair<string, string>* p = OptFrameFunction::run_function(last, allFunctions, dictionary, ldictionary, scanFunc.rest());

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

#endif /* OPTFRAMEMODULE_HPP_ */
