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

#ifndef OPTFRAME_LIST_REMOVE_MODULE_HPP_
#define OPTFRAME_LIST_REMOVE_MODULE_HPP_

#include<string>

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ListRemoveModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ListRemoveModule()
	{
	}

	string id()
	{
		return "list_remove";
	}

	string usage()
	{
		return "list_remove list index new_list_name";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>  list;
		vector<string>* p_list = OptFrameList::readList(ldictionary, scanner);
		if(p_list)
		{
			list = vector<string>(*p_list);
			delete p_list;
		}
		else
			return false;

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		int index = scanner.nextInt();
		index--; // index is [1..size()], but in c++ is [0..size()-1]

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string new_name = Scanner::trim(scanner.next());

		if((new_name != "") && (new_name.at(0)=='['))
		{
			cout << "list_remove module: invalid list new name '" << new_name << "'" << endl;
			return false;
		}

		if( (index < 0) || (index >= ((int)list.size())) )
		{
			cout << "list_remove module: invalid index '" << index << "'!" << endl;
			return false;
		}

		list.erase(list.begin()+index);

		stringstream ss;
		ss << new_name << " " << OptFrameList::listToString(list);

		//TODO: acessar dicionario de listas diretamente!! Bem mais eficiente!

		return OptFrameModule<R, ADS, M>::run_module("silent_define_list", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
	}

};

#endif /* OPTFRAME_LIST_REMOVE_MODULE_HPP_ */
