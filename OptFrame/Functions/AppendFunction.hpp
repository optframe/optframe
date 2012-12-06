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

	virtual pair<string, string>* run(vector<OptFrameFunction*>& allFunctions, map< string, string >&, map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		vector<string>* plist1 = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list1;
		if(plist1)
		{
			list1 = vector<string>(*plist1);
			delete plist1;
		}
		else
			return NULL;


		vector<string>* plist2 = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list2;
		if(plist2)
		{
			list2 = vector<string>(*plist2);
			delete plist2;
		}
		else
			return NULL;


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

		return new pair<string, string>(list3, scanner.rest());
	}
};

#endif /* OPTFRAME_APPEND_FUNCTION_HPP_ */
