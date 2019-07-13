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
 * PreprocessFunction<R,ADS,DS>.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_FUNCTION_HPP_
#define OPTFRAME_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "Solution.hpp"
#include "Evaluation.hpp"

#include "Scanner++/Scanner.h"

#include <algorithm>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class PreprocessFunction
{
protected:
	static int precision; // for floats and doubles

public:

	static pair<string, string>* run_function(string func, vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string raw_input)
	{
		string input = Scanner::trim(raw_input);

		//printf("run_function: '%s' with: '%s'\n", func.c_str(), input.c_str());

		for(unsigned int i=0;i<allFunctions.size();i++)
			if(allFunctions[i]->canHandle(func, "")) // TODO: check
			{
				//cout << "found function: '" << func << "'" << endl;
				string* iprep = allFunctions[i]->preprocess(allFunctions, hf, dictionary, ldictionary, input);
				if(!iprep)
					return nullptr;

				//printf("function_after_prep (%s): '%s'\n", func.c_str(), iprep->c_str());

				string body = "";
				int index = -1;
				bool inString = false;
				int countBrackets = 0;
				for(int j=0; j< ((int)iprep->length()); j++)
				{
					if(!inString && iprep->at(j) == '"') // start string
					{
						inString = true;
						body += '"';
					}
					else if(inString && iprep->at(j) == '"') // finish string
					{
						inString = false;
						body += '"';
					}
					else if(inString)
						body += iprep->at(j); // usual character
					else if(!inString && (iprep->at(j)=='('))
					{
						countBrackets++;
						body += iprep->at(j);
					}
					else if(!inString &&  (iprep->at(j)==')') && (countBrackets>0)) // usual character
					{
						countBrackets--;
						body += iprep->at(j);
					}
					else if((iprep->at(j)==')') && (countBrackets==0)) // out of string and close function
					{
						index = j;
						break;
					}
					else
						body += iprep->at(j); // usual character
				}

				//printf("body (%s): '%s'\n", func.c_str(), body.c_str());

				string rest = "";
				if((index != -1) && (index < ((int)iprep->length())-1))
					for(int j=index+1; j< ((int)iprep->length()); j++)
						rest += iprep->at(j);

				//printf("rest (%s): '%s'\n", func.c_str(), rest.c_str());

				delete iprep;

				string* r = allFunctions[i]->run(allFunctions, hf, dictionary, ldictionary, body);

				if(!r)
					return nullptr;
				else
				{
					string r1 = *r;
					delete r;
					return new pair<string, string>(r1, rest);
				}
			}

		cout << "Function '" << func << "' not found." << endl;

		return nullptr;
	}

	static bool functionExists(string func, vector<PreprocessFunction<R,ADS,DS>*>& allFunctions)
	{
		for(unsigned int i=0;i<allFunctions.size();i++)
			if(allFunctions[i]->canHandle(func, "")) // TODO: check
				return true;
		return false;
	}

	virtual ~PreprocessFunction()
	{
	}

	vector<string> handles;

	virtual bool canHandle(string f_name, string f_body)
	{
		if (id() == f_name)
			return true;

		for (unsigned i = 0; i < handles.size(); i++)
			if (handles[i] == f_name)
				return true;

		return false;
	}

	virtual string id() = 0;

	virtual string usage() = 0;

	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body) = 0;

	// SHOULD NOT REIMPLEMENT!! TO BE REMOVED IN FUTURE (TODO)
	string* preprocess(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string input)
	{
		return defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

	static string* defaultPreprocess(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		// Locate functions

		if(!scanner.hasNext())
			return new string(input); // empty

		int bcount = 0;
		stringstream ssfunc;
		// add space before and after brackets '(' ')'
		for(unsigned i=0; i<input.length(); i++)
			if(input.at(i)=='(')
			{
				ssfunc << " " << input.at(i) << " ";
				bcount++;
			}
			else if(input.at(i)==')')
			{
				ssfunc << " " << input.at(i) << " ";
			}
			else
				ssfunc << input.at(i);

		if(bcount==0)
			return new string(input); // no functions

		Scanner scanFunc(ssfunc.str());

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
					cout << "function error in '" << last << "'" << endl;
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
int PreprocessFunction<R,ADS,DS>::precision=2;

}

#endif /* OPTFRAME_FUNCTION_HPP_ */
