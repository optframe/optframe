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

#ifndef OPTFRAME_COMPONENT_LOG_HPP_
#define OPTFRAME_COMPONENT_LOG_HPP_

#include "../Command.hpp"

#include <stdio.h>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentLogCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ComponentLogCommand()
	{
	}

	string id()
	{
		return "component.log";
	}

	string usage()
	{
		string out = "component.log component_type id variable_to_store_log";

		return out;
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		Component* comp = factory.getNextComponent(scanner);

		if(!comp)
		{
			cout << id() << " error: NULL COMPONENT!" << endl;
			return false;
		}

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string var = scanner.next();

		stringstream ss;
		ss << var << " " << comp->log();

		return Command<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* OPTFRAME_COMPONENT_LOG_HPP_ */
