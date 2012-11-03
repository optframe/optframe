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

#ifndef LISTFROMPOPULATIONMODULE_HPP_
#define LISTFROMPOPULATIONMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ListFromPopulationModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ListFromPopulationModule()
	{
	}

	string id()
	{
		return "list_from_population";
	}
	string usage()
	{
		return "list_from_population new_list_name loadpop id";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		Scanner scan(input);
		if (!scan.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string listName = scan.next();

		if (!scan.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string strloadpop = scan.next();

		if (strloadpop != "loadpop")
		{
			cout << "expected 'loadpop id', but found '" << strloadpop << "'" << endl;
			return;
		}

		string id = scan.next();
		Scanner pop(strloadpop + " " + id);
		Population<R, ADS>* p = NULL;
		factory->readComponent(p, &pop);

		stringstream listContent;

		listContent << "[ ";

		for (unsigned i = 0; i < p->size() - 1; i++)
		{
			int sid = factory->addComponent(p->at(i).clone());
			listContent << "loadsol " << sid << " , ";
		}

		if (p->size() > 0)
		{
			unsigned i = p->size() - 1;
			int sid = factory->addComponent(p->at(i).clone());
			listContent << "loadsol " << sid;
		}

		listContent << " ]";

		OptFrameModule<R, ADS, M>::run_module("define", all_modules, factory, dictionary, listName + " " + listContent.str());
	}

};

#endif /* LISTFROMPOPULATIONMODULE_HPP_ */
