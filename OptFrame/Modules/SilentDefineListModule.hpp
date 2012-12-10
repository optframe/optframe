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

#ifndef SILENT_DEFINE_LIST_MODULE_HPP_
#define SILENT_DEFINE_LIST_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class SilentDefineListModule : public OptFrameModule<R, ADS, M>
{
public:

	virtual ~SilentDefineListModule()
	{
	}

	string id()
	{
		return "silent_define_list";
	}
	string usage()
	{
		return "silent_define_list name list";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>&, vector<OptFrameFunction*>&, HeuristicFactory<R, ADS, M>&, map<string,string>&, map< string,vector<string> >& ldictionary, string rest)
	{
		Scanner scanner(rest);

		string list_name = scanner.next();

		if(list_name != "")
		{
			vector<string>  list;
			vector<string>* p_list = OptFrameList::readList(ldictionary, scanner);
			if(p_list)
			{
				list = vector<string>(*p_list);
				delete p_list;
			}
			else
			{
				cout << "silent_define_list module: couldn't load list!" << endl;
				//cout << "rest: '" << rest << "'" << endl;
				return false;
			}


			ldictionary[list_name] = list;
			return true;
		}
		else
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}
	}

	virtual string* preprocess(vector<OptFrameFunction*>& allFunctions, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input)
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

		// Second, use the dictionary, but before...
		// WAIT! Save the definition name!

		if(!scanner.hasNext())
			return new string(input2);

		string name      = scanner.next();
		string discarded = scanner.getDiscarded();

		// now proceed as usual

		string* input3 = OptFrameModule<R, ADS, M>::defaultPreprocess(allFunctions, dictionary, ldictionary, scanner.rest());

		if(!input3)
			return NULL;

		string input4;
		input4.append(discarded);
		input4.append(name);
		input4.append(" ");
		input4.append(*input3);

		delete input3;

		string input5 = Scanner::trim(input4);

		return new string(input5);
	}


};

#endif /* SILENT_DEFINE_LIST_MODULE_HPP_ */
