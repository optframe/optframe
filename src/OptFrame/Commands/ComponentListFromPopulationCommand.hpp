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

#ifndef LISTFROMPOPULATIONMODULE_HPP_
#define LISTFROMPOPULATIONMODULE_HPP_

#include "../Command.hpp"

#include "SystemDefineCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentListFromPopulationCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ComponentListFromPopulationCommand()
	{
	}

	string id()
	{
		return "component.list_from_population";
	}

	string usage()
	{
		return "component.list_from_population new_list_name loadpop id";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scan(input);
		if (!scan.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string listName = scan.next();

		if (!scan.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string strloadpop = scan.next();

		if (strloadpop != "loadpop")
		{
			cout << "expected 'loadpop id', but found '" << strloadpop << "'" << endl;
			return false;
		}

		string id = scan.next();
		Scanner scan_pop(strloadpop + " " + id);
		Population<R, ADS>* p = nullptr;
		factory.readComponent(p, scan_pop);

		stringstream listContent;

		listContent << "[ ";

		for (unsigned i = 0; i < p->size() - 1; i++)
		{
			int sid = factory.addComponent(p->at(i).clone());
			listContent << "loadsol " << sid << " , ";
		}

		if (p->size() > 0)
		{
			unsigned i = p->size() - 1;
			int sid = factory.addComponent(p->at(i).clone());
			listContent << "loadsol " << sid;
		}

		listContent << " ]";

		return Command<R, ADS, DS>::run_module("system.define", all_modules, allFunctions, factory, dictionary, ldictionary, listName + " " + listContent.str());
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* LISTFROMPOPULATIONMODULE_HPP_ */
