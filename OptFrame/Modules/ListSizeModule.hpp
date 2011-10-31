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

#ifndef LISTSIZEMODULE_HPP_
#define LISTSIZEMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ListSizeModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ListSizeModule()
	{
	}

	string id()
	{
		return "list_size";
	}
	string usage()
	{
		return "list_size list variable_for_list_size";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* hf, map<string, string>* dictionary, string input)
	{
		Scanner scan(input);
		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		vector < string > list = hf->readList(scan);

		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string variable = scan.next();

		stringstream nameAndSize;
		nameAndSize << variable << " " << list.size();

		run_module("define", all_modules, hf, dictionary, nameAndSize.str());
	}

};

#endif /* ELEMENTMODULE_HPP_ */
