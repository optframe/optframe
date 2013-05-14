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

#ifndef CREATE_LIST_OF_COMPONENTS_MODULE_HPP_
#define CREATE_LIST_OF_COMPONENTS_MODULE_HPP_

#include "../Module.hpp"

#include "SystemSilentDefineModule.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentCreateListModule: public Module<R, ADS, DS>
{
public:

	virtual ~ComponentCreateListModule()
	{
	}

	string id()
	{
		return "component.create_list";
	}

	string usage()
	{
		return "component.create_list list type list_name";
	}

	bool run(vector<Module<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		vector<string>* plist = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list;
		if(plist)
		{
			list = vector<string>(*plist);
			delete plist;
		}
		else
		{
			cout << "module " << id() << " failed to read list!" << endl;
			return false;
		}

		string type = scanner.next();
		string name = scanner.next();

		vector<OptFrameComponent*> componentList;
		for(unsigned i=0; i<list.size(); i++)
		{
			Scanner scan(list[i]);
			OptFrameComponent* comp = factory.getNextComponent(scan);
			if(!comp)
			{
				cout << "create_list_of_components: error, component #" << i << " is NULL! " << endl;
				return false;
			}
			else if(!comp->compatible(factory.typeOfList(type)))
			{
				cout << "create_list_of_components: error, component #" << i << " ('" << comp->id() <<"') in list incompatible with type '" << factory.typeOfList(type) << "'" << endl;
				return false;
			}
			else
				componentList.push_back(comp);
		}

		int idx = factory.addComponentList(componentList, type);

		stringstream ss;

		ss << name << " " << factory.typeOfList(type) << "[] " << idx;

		cout << "'" << factory.typeOfList(type) << "[] " << idx << "' added." << endl;

		return Module<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Module<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* CREATE_LIST_OF_COMPONENTS_MODULE_HPP_ */
