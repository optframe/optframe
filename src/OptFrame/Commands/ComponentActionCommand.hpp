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

#ifndef OPTFRAME_COMPONENT_ACTION_HPP_
#define OPTFRAME_COMPONENT_ACTION_HPP_

#include "../Command.hpp"

#include "SystemSilentDefineCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentActionCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ComponentActionCommand()
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

	bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "component.action: " << input << endl;

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;

			cout << factory.actions.size() << " available action handlers:" << endl;
			cout << "===============================================" << endl;
			cout << "Component index  |  action  | action parameters" << endl;
			cout << "===============================================" << endl;

			for (unsigned a = 0; a < factory.actions.size(); a++)
				cout << factory.actions[a]->usage() << endl << endl;

			cout << "===============================================" << endl;

			return false;
		}

		string compName; // component name (or translated shortcut)
		int compNumber;  // component index
		Component* comp = factory.getNextComponent(scanner, &compName, &compNumber);

		if(!comp)
		{
			cout << "module " << id() << " error: nullptr component!" << endl;
			return false;
		}

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " error: missing action name!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string action = scanner.next();

		//cout << "FOUND COMPONENT '" << compName << "' and action '" << action << "'" << endl;
		//cout << "will look for action: '" << action << "'" << endl;

		for(unsigned a=0; a<factory.actions.size(); a++)
			if(factory.actions[a]->handleComponent(compName) && factory.actions[a]->handleAction(action))
			{
				stringstream ssaction;
				ssaction << compName << " " << compNumber << " " << action << scanner.rest();

				return factory.actions[a]->doAction(ssaction.str(), factory, dictionary, ldictionary);
			}

        cout << "component.action error: component '" << compName << "' and action '" << action << "' not found!" << endl;
		return false;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* OPTFRAME_COMPONENT_ACTION_HPP_ */
