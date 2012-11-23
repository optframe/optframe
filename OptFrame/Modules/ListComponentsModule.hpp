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

#ifndef LIST_COMPONENTS_MODULE_HPP_
#define LIST_COMPONENTS_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ListComponentsModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ListComponentsModule()
	{
	}

	string id()
	{
		return "list_components";
	}

	string usage()
	{
		return "list_components pattern [store_list]\nWhere: store_list is an optional variable to store the components.";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, string input)
	{
		Scanner scanner(input);

		string pattern = scanner.next();

		vector<string> components     = factory.listComponents(pattern);
		vector<string> componentLists = factory.listComponentLists(pattern);

		components.insert(components.end(), componentLists.begin(), componentLists.end());

		if(!scanner.hasNext())
		{
			for(int i=0; i<(int)components.size(); i++)
				cout << components[i] << endl;

			return true;
		}
		else
		{
			string new_name = scanner.next();
			stringstream ss;

			ss << new_name << " [";
			for(unsigned i=0; i<components.size(); i++)
			{
				ss << components[i];
				if(i != components.size()-1)
					ss << ",";
			}
			ss << " ]";

			return OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, allFunctions, factory, dictionary, ss.str());
		}

	}

};

#endif /* LIST_COMPONENTS_MODULE_HPP_ */
