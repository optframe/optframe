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

#ifndef OPTFRAME_COMPONENT_LIST_MODULE_HPP_
#define OPTFRAME_COMPONENT_LIST_MODULE_HPP_

#include "../OptFrameModule.hpp"

#include "SystemDefineModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ComponentListModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ComponentListModule()
	{
	}

	string id()
	{
		return "component.list";
	}

	string usage()
	{
		return "component.list pattern [store_list]\nWhere: store_list is an optional variable to store the components.";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if(!scanner.hasNext())
		{
			return false;
		}

		string pattern = scanner.next();

		vector<string> components     = factory.listComponents(pattern);
		vector<string> componentLists = factory.listComponentLists(pattern);

		components.insert(components.end(), componentLists.begin(), componentLists.end());

		//cout << "component.list module: NUM. COMPONENTS IS " << components.size() << endl;

		if(!scanner.hasNext())
		{
			for(int i=0; i<(int)components.size(); i++)
				cout << components[i] << endl;

			return true;
		}
		else
		{
			string new_name = scanner.next();

			//cout << "component.list module: NEW_NAME = '" << new_name << "'" << endl;

			stringstream ss;

			//cout << "components.list module: COMPONENTS '" << components << "'" << endl;

			ss << new_name << " " << OptFrameList::listToString(components);

			//cout << "component.list module: CREATING LIST OF COMPONENTS '" << ss.str() << "'" << endl;

			return OptFrameModule<R, ADS, M>::run_module("list.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
		}

	}

	// disable preprocess to don't destroy type!
	virtual string* preprocess(vector<OptFrameFunction*>&, map<string, string>&,  map< string,vector<string> >&, string input)
	{
		// disable preprocess!!
		return new string(input);
	}

};

#endif /* OPTFRAME_COMPONENT_LIST_MODULE_HPP_ */
