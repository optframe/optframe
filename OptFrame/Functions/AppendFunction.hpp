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

#ifndef OPTFRAME_APPEND_FUNCTION_HPP_
#define OPTFRAME_APPEND_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class AppendFunction : public OptFrameFunction
{
public:

	virtual ~AppendFunction()
	{
	}

	virtual string id()
	{
		return "append";
	}

	virtual string usage()
	{
		return "append( list1 list2 ) : return append list1 with list2";
	}

	virtual pair<string, string> run(vector<OptFrameFunction*>& allFunctions, string body)
	{
		Scanner scanner(body);

		vector<string> list1 = OptFrameList::readList(scanner);
		vector<string> list2 = OptFrameList::readList(scanner);

		list1.insert(list1.end(), list2.begin(), list2.end());

		stringstream ss;
		ss << "[ ";
		for(unsigned i=0; i<list1.size(); i++)
		{
			ss << list1[i];
			if(i != list1.size()-1)
				ss << ",";
		}
		ss << " ]";

		string list3 = ss.str();

		scanner.next(); // drop ')'

		return make_pair(list3, scanner.rest());
	}
};

#endif /* OPTFRAME_APPEND_FUNCTION_HPP_ */
