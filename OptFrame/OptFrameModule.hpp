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

#include "HeuristicFactory.hpp"

#include <algorithm>

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class OptFrameModule
{
protected:
	void run_module(string mod, vector<OptFrameModule<R,M>*> v, HeuristicFactory<R,M>* f, map<string,string>* dictionary, string input)
	{
		for(unsigned int i=0;i<v.size();i++)
			if(mod==v[i]->id())
			{
				v[i]->run(v,f,dictionary,input);
				return;
			}
		cout << "Module '"<<mod<<"' not found." << endl;
	}

public:
	virtual string id() = 0;
	virtual string usage() = 0;

	virtual void run(vector<OptFrameModule<R,M>*>&, HeuristicFactory<R,M>*, map<string,string>* dictionary, string) = 0;

	virtual string preprocess(map<string,string>* dictionary, string input)
	{
		return defaultPreprocess(dictionary,input);
	}

	static string defaultPreprocess(map<string,string>* dictionary, string input)
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

		return input4;
	}
};

#endif /* OPTFRAMEMODULE_HPP_ */
