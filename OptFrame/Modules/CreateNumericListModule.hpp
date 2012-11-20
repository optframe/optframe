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

#ifndef CREATE_NUMERIC_LIST_MODULE_HPP_
#define CREATE_NUMERIC_LIST_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class CreateNumericListModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~CreateNumericListModule()
	{
	}

	string id()
	{
		return "create_numeric_list";
	}
	string usage()
	{
		return "create_numeric_list begin end list_name";
	}
	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		Scanner scanner(input);

		int begin    = scanner.nextInt();
		int end      = scanner.nextInt();
		string lname = scanner.next();

		stringstream ss;

		ss << lname << " [";
		for(int i=begin; i<end; i++)
			ss << i << ",";
		if((end-begin)>=0)
			ss << end;
		ss << " ]";

		return OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, allFunctions, factory, dictionary, ss.str());
	}

};

#endif /* CREATE_NUMERIC_LIST_MODULE_HPP_ */
