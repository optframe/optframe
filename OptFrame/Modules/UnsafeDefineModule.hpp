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

#ifndef UNSAFE_DEFINE_MODULE_HPP_
#define UNSAFE_DEFINE_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class UnsafeDefineModule : public OptFrameModule<R, ADS, M>
{
public:

	virtual ~UnsafeDefineModule()
	{
	}

	string id()
	{
		return "system.unsafe_define";
	}
	string usage()
	{
		return "system.unsafe_define new_name text";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>&, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>&, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string rest)
	{
		Scanner scanner(rest);

		string new_name = scanner.next();

		if(new_name != "")
		{
			string second_word = scanner.rest();

			Scanner s2(second_word);

			while(s2.hasNext())
			{
				string next_word = s2.next();
				if(new_name==next_word)
				{
					cout << "unsafe_define module: recursive definitions are not allowed! (define: '" << new_name << "' as '" << next_word << "')" << endl;
					return false;
				}
			}

			//cout << "unsafe_define module: word '" << new_name << "' -> '" << second_word << "'" << endl;

			dictionary[new_name] = scanner.trim(second_word);
			return true;
		}
		else
		{
			cout << "Usage: "<<usage()<<endl;
			return false;
		}
	}

	// FAITH ON USER!! NO PREPROCESSING :D
	virtual string* preprocess(vector<OptFrameFunction*>& allFunctions, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		return new string(input);
	}

};

#endif /* UNSAFE_DEFINE_MODULE_HPP_ */
