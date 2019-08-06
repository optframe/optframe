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

#ifndef OPTFRAME_COMPONENT_CAST_HPP_
#define OPTFRAME_COMPONENT_CAST_HPP_

#include "../Command.hpp"

#include "SystemSilentDefineCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentCastCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ComponentCastCommand()
	{
	}

	string id()
	{
		return "component.cast";
	}

	string usage()
	{
		string u = "component.cast   Component id   new_type  [variable]";
		return u;
	}

	bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		//cout << "component.cast: " << input << endl;

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string comp = scanner.next();

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		int number = scanner.nextInt();

		if (!scanner.hasNext())
		{
			cout << "module " << id() << " error: missing type name!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string type = scanner.next();

		string variable = "";
		if (scanner.hasNext())
			variable = scanner.next();

		//cout << "will look for action: '" << type << "'" << endl;

		for (unsigned a = 0; a < factory.actions.size(); a++)
			if (factory.actions[a]->handleComponent(type))
			{
				return factory.actions[a]->doCast(comp, number, type, variable, factory, dictionary);
			}

		cout << "component.cast error: action with cast handle for type '" << type << "' not found!" << endl;
		return false;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* OPTFRAME_COMPONENT_CAST_HPP_ */
