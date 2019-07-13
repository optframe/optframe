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

#include "../PreprocessFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ListAppendFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~ListAppendFunction()
	{
	}

	virtual string id()
	{
		return "list.append";
	}

	virtual string usage()
	{
		return "list.append( list1 list2 ) : return append list1 with list2";
	}

	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>&, const map< string, string >&, const map< string,vector<string> >& ldictionary, string body)
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
			return nullptr;


		vector<string>* plist2 = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list2;
		if(plist2)
		{
			list2 = vector<string>(*plist2);
			delete plist2;
		}
		else
			return nullptr;


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

		return new string(list3);
	}
};

}

#endif /* OPTFRAME_APPEND_FUNCTION_HPP_ */
