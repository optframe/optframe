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

#ifndef OPTFRAME_COMPONENT_LIST_MODULE_HPP_
#define OPTFRAME_COMPONENT_LIST_MODULE_HPP_

#include "../Command.hpp"

#include "SystemDefineCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentListCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ComponentListCommand()
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

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		string pattern = "OptFrame:";

		if (scanner.hasNext())
		{
			pattern = scanner.next();
		}

		vector<string> components     = factory.listComponents(pattern);
		vector<string> componentLists = factory.listComponentLists(pattern);

		components.insert(components.end(), componentLists.begin(), componentLists.end());

		//cout << "component.list command: NUM. COMPONENTS IS " << components.size() << endl;

		if(!scanner.hasNext())
		{
			for(int i=0; i<(int)components.size(); i++)
				cout << components[i] << endl;

			return true;
		}
		else
		{
			string new_name = scanner.next();

			//cout << "component.list command: NEW_NAME = '" << new_name << "'" << endl;

			stringstream ss;

			//cout << "components.list command: COMPONENTS '" << components << "'" << endl;

			ss << new_name << " " << OptFrameList::listToString(components);

			//cout << "component.list command: CREATING LIST OF COMPONENTS '" << ss.str() << "'" << endl;

			return Command<R, ADS, DS>::run_module("list.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
		}

	}

	// disable preprocess to don't destroy type!
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		// disable preprocess!!
		return new string(input);
	}

};

}

#endif /* OPTFRAME_COMPONENT_LIST_MODULE_HPP_ */
