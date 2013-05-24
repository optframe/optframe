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

#ifndef OPTFRAME_COMPONENT_ACTION_HPP_
#define OPTFRAME_COMPONENT_ACTION_HPP_

#include "../Module.hpp"

#include "SystemSilentDefineModule.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentActionModule: public Module<R, ADS, DS>
{
public:

	virtual ~ComponentActionModule()
	{
	}

	string id()
	{
		return "component.action";
	}

	string usage()
	{
		string u = "component.action   Component id   action_name   specific_action_parameters";
		return u;
	}

	bool run(vector<Module<R, ADS, DS>*>& allModules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "component.action: " << input << endl;

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		OptFrameComponent* comp = factory.getNextComponent(scanner);

		if(!comp)
		{
			cout << "module " << id() << " error: NULL component!" << endl;
			return false;
		}

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " error: missing action name!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string action = scanner.next();

		//cout << "will look for action: '" << action << "'" << endl;

		for(unsigned a=0; a<factory.actions.size(); a++)
			if(factory.actions[a]->handleComponent(*comp) && factory.actions[a]->handleAction(action))
			{
				return factory.actions[a]->doAction(input, factory, dictionary, ldictionary);
			}

		return false;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Module<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* OPTFRAME_COMPONENT_ACTION_HPP_ */
