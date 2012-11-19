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

#ifndef OPTFRAME_AVG_FUNCTION_HPP_
#define OPTFRAME_AVG_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class AvgFunction : public OptFrameFunction
{
public:

	virtual ~AvgFunction()
	{
	}

	virtual string id()
	{
		return "avg";
	}

	virtual string usage()
	{
		return "avg( list ) : return list average";
	}

	virtual pair<string, string> run(vector<OptFrameFunction*>& allFunctions, string body)
	{
		Scanner scanner(body);

		vector<string> list = OptFrameList::readList(scanner);

		if(list.size()==0)
			return make_pair("","");

		double sum = 0;
		for(unsigned i=0; i<list.size(); i++)
			sum += Scanner::parseDouble(list[i]);

		int len = list.size();

		stringstream ss;
		ss << (sum/len);

		string avg = ss.str();

		scanner.next(); // drop ')'

		return make_pair(avg, scanner.rest());
	}
};

#endif /* OPTFRAME_AVG_FUNCTION_HPP_ */
