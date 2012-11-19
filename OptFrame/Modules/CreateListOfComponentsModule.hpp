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

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class CreateListOfComponentsModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~CreateListOfComponentsModule()
	{
	}

	string id()
	{
		return "create_list_of_components";
	}
	string usage()
	{
		return "create_list_of_components list type list_name";
	}
	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		Scanner scanner(input);

		vector < string > list = factory->readList(scanner);
		string type = scanner.next();
		string name = scanner.next();

		vector<OptFrameComponent*> componentList;
		for(unsigned i=0; i<list.size(); i++)
		{
			Scanner scan(list[i]);
			OptFrameComponent* comp = factory->getNextComponent(scan);
			if(!comp)
			{
				cout << "create_list_of_components: error, component #" << i << " is NULL! " << endl;
				return;
			}
			else if(!comp->compatible(factory->typeOfList(type)))
			{
				cout << "create_list_of_components: error, component #" << i << " ('" << comp->id() <<"') in list incompatible with type '" << factory->typeOfList(type) << "'" << endl;
				return;
			}
			else
				componentList.push_back(comp);
		}

		int idx = factory->addComponentList(componentList, type);

		stringstream ss;

		ss << name << " " << factory->typeOfList(type) << "[] " << idx;

		cout << "'" << factory->typeOfList(type) << "[] " << idx << "' added." << endl;

		OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, allFunctions, factory, dictionary, ss.str());
	}

};

#endif /* CREATE_LIST_OF_COMPONENTS_MODULE_HPP_ */
