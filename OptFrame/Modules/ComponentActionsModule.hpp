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

#ifndef OPTFRAME_COMPONENT_ACTIONS_HPP_
#define OPTFRAME_COMPONENT_ACTIONS_HPP_

#include "../Module.hpp"

#include "SystemSilentDefineModule.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentActionsModule: public Module<R, ADS, DS>
{
public:

	virtual ~ComponentActionsModule()
	{
	}

	string id()
	{
		return "component.actions";
	}

	string usage()
	{
		string u = "component.actions";
		return u;
	}

	bool run(vector<Module<R, ADS, DS>*>& allModules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		cout << factory.actions.size() << " available action handlers:" << endl;
		cout << "===============================================" << endl;
		cout << "Component index  |  action  | action parameters" << endl;
		cout << "===============================================" << endl;

		for (unsigned a = 0; a < factory.actions.size(); a++)
			cout << factory.actions[a]->usage() << endl << endl;

		cout << "===============================================" << endl;

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Module<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* OPTFRAME_COMPONENT_ACTIONS_HPP_ */
