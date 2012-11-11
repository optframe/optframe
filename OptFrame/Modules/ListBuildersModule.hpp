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

#ifndef LIST_BUILDERS_MODULE_HPP_
#define LIST_BUILDERS_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ListBuildersModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ListBuildersModule()
	{
	}

	string id()
	{
		return "list_builders";
	}

	string usage()
	{
		return "list_builders pattern [store_list]\nWhere: store_list is an optional variable to store the builders.";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		Scanner scanner(input);

		string pattern = scanner.next();

		vector<pair<string, vector<pair<string,string> > > > builders = factory->listBuilders(pattern);

		if(!scanner.hasNext())
		{
			for(int i=0; i<(int)builders.size(); i++)
			{
				cout << builders[i].first << " ";
				for(unsigned j=0; j<builders[i].second.size(); j++)
					cout << builders[i].second[j].first << " " << builders[i].second[j].second << " ";
				cout << endl;
			}
		}
		else
		{
			string new_name = scanner.next();
			stringstream ss;

			ss << new_name << " [";
			for(int i=0; i<((int)builders.size())-1; i++)
				ss << builders[i].first << ",";
			if(builders.size()>0)
				ss << builders[builders.size()-1].first;
			ss << " ]";

			OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, factory, dictionary, ss.str());
		}

	}

};

#endif /* LIST_BUILDERS_MODULE_HPP_ */
