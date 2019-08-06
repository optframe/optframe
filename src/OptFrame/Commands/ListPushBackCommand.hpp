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

#ifndef OPTFRAME_LIST_PUSH_BACK_MODULE_HPP_
#define OPTFRAME_LIST_PUSH_BACK_MODULE_HPP_

#include<string>

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ListPushBackCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ListPushBackCommand()
	{
	}

	string id()
	{
		return "list.push_back";
	}

	string usage()
	{
		return "list.push_back list_name value";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "list.push_back command: '" << input << "'" << endl;

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string list_name = scanner.next();

		if(list_name.at(0) == '[')
		{
			cout << "module " << id() << " error: cannot be used with explicit list! use list.add instead." << endl;
			return false;
		}

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string element = Scanner::trim(scanner.rest());

		Scanner scanList(list_name);

		vector<string>  list;
		vector<string>* p_list = OptFrameList::readList(ldictionary, scanList);
		if(p_list)
		{
			list = vector<string>(*p_list);
			delete p_list;
		}
		else
		{
			cout << "list.push_back error reading list '" << list_name << "'" << endl;
			return false;
		}

		list.push_back(element);

		return Command<R, ADS, DS>::defineList(list_name, list, ldictionary);
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}
};

}

#endif /* OPTFRAME_LIST_PUSH_BACK_MODULE_HPP_ */
