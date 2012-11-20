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
	bool run_module(string mod, vector<OptFrameModule<R, ADS, M>*> v, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>* f, map<string,string>* dictionary, string input)
	{
		for(unsigned int i=0;i<v.size();i++)
			if(mod==v[i]->id())
				return v[i]->run(v, allFunctions, f, dictionary, input);

		cout << "Module '"<<mod<<"' not found." << endl;
		return false;
	}

public:

	virtual ~OptFrameModule()
	{
	}

	virtual string id() = 0;
	virtual string usage() = 0;

	virtual bool run(vector<OptFrameModule<R, ADS, M>*>&, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>*, map<string,string>* dictionary, string) = 0;

	virtual string preprocess(vector<OptFrameFunction*>& allFunctions, map<string,string>* dictionary, string input)
	{
		return defaultPreprocess(allFunctions, dictionary,input);
	}

	static string defaultPreprocess(vector<OptFrameFunction*>& allFunctions, map<string,string>* dictionary, string input)
	{
		Scanner scanner(input);

		// First, remove the comments '%'

		string input2 = "";

		while(scanner.hasNextChar())
		{
			char c = scanner.nextChar();
			if(c != '%')
				input2 += c;
			else
				break;
		}

		scanner = Scanner(input2);

		// Second, use the dictionary

		string input3 = "";

		while(scanner.hasNext())
		{
			string new_word = scanner.next();
			string unused = scanner.getDiscarded();

			if(dictionary->count(new_word) == 0) // Not found in dictionary!
			{
				input3.append(unused);
				input3.append(new_word);
			}
			else
			{
				string found = dictionary->find(new_word)->second;

				input3.append(unused);
				input3.append(found);
				input3.append(scanner.rest());

				scanner = Scanner(input3);
				input3 = "";
			}
		}

		string input4 = Scanner::trim(input3);

		// Third, locate functions

		stringstream ssfunc;
		// add space before and after brackets '(' ')'
		for(unsigned i=0; i<input4.length(); i++)
			if((input4.at(i)=='(') || (input4.at(i)==')'))
				ssfunc << " " << input4.at(i) << " ";
			else
				ssfunc << input4.at(i);

		Scanner scanFunc(ssfunc.str());

		if(!scanFunc.hasNext())
			return input4; // no functions

		string input5 = "";

		string last       = scanFunc.next();
		string ldiscarded = scanFunc.getDiscarded();

		while(scanFunc.hasNext())
		{
			string current    = scanFunc.next();
			string cdiscarded = scanFunc.getDiscarded();

			if((current == "(") && OptFrameFunction::functionExists(last, allFunctions)) // FUNCTION
			{
				pair<string, string> p = OptFrameFunction::run_function(last, allFunctions, scanFunc.rest());

				input5.append(" ");
				input5.append(p.first);
				input5.append(" ");

				scanFunc = Scanner(p.second);

				if(!scanFunc.hasNext())
				{
					last = "";
					ldiscarded = "";
					break;
				}
				else
				{
					last    = scanFunc.next();
					ldiscarded = scanFunc.getDiscarded();
					continue;
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

		return input6;
	}
};

#endif /* OPTFRAMEMODULE_HPP_ */
