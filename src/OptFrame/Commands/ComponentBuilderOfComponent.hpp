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

#ifndef LIST_BUILDER_OF_COMPONENT_MODULE_HPP_
#define LIST_BUILDER_OF_COMPONENT_MODULE_HPP_

#include "../Command.hpp"

#include "SystemSilentDefineCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentBuilderOfComponentCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ComponentBuilderOfComponentCommand()
	{
	}

	string id()
	{
		return "component.builder_of_component";
	}

	string usage()
	{
		return "component.builder_of_component component_name [store_list]\nWhere: store_list is an optional variable to store the builders.";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		string component_name = scanner.next();

		string pattern = "OptFrame:ComponentBuilder:";
		// SKIP "OptFrame:" and replace by "OptFrame:ComponentBuilder:"
		for(unsigned i=Component::idComponent().length(); i<component_name.size(); i++)
			pattern += component_name.at(i);

		vector<pair<string, vector<pair<string,string> > > > builders = factory.listBuilders(pattern);

		if(!scanner.hasNext())
		{
			for(int i=0; i<(int)builders.size(); i++)
			{
				cout << builders[i].first << "\t";
				for(unsigned j=0; j<builders[i].second.size(); j++)
					cout << builders[i].second[j].first << "=>'" << builders[i].second[j].second << "' ";
				cout << endl;
			}

			return true;
		}
		else
		{
			string new_name = scanner.next();
			stringstream ss;

			ss << new_name << " [";
			for(unsigned i=0; i<builders.size(); i++)
			{
				ss << " [ " << builders[i].first << " , [ ";
				for(unsigned j=0; j<builders[i].second.size(); j++)
				{
					ss << "[ " << builders[i].second[j].first << " , " << builders[i].second[j].second << "] ";
					if(j != builders[i].second.size()-1)
						ss << ",";
				}
				ss << " ] ] ";

				if( i != builders.size()-1 )
					ss << ",";
			}
			ss << " ]";

			return Command<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
		}
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* LIST_BUILDER_OF_COMPONENT_MODULE_HPP_ */
