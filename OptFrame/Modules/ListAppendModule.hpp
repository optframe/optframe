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

#ifndef LIST_APPEND_MODULE_HPP_
#define LIST_APPEND_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ListAppendModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ListAppendModule()
	{
	}

	string id()
	{
		return "list_append";
	}
	string usage()
	{
		return "list_append list1 list2 name_for_final_list";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* hf, map<string, string>* dictionary, string input)
	{
		Scanner scan(input);
		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		vector < string > list1 = hf->readList(scan);

		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		vector < string > list2 = hf->readList(scan);

		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string name = scan.next();

		list1.insert(list1.end(), list2.begin(), list2.end());

		stringstream ss;
		ss << name << " [ ";
		for(unsigned i=0; i<list1.size(); i++)
		{
			ss << list1[i];
			if(i != list1.size()-1)
				ss << ",";
		}
		ss << " ]";

		OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, hf, dictionary, ss.str());
	}

};

#endif /* LIST_APPEND_MODULE_HPP_ */
