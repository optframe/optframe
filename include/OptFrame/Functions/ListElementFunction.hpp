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

#ifndef OPTFRAME_ELEMENT_FUNCTION_HPP_
#define OPTFRAME_ELEMENT_FUNCTION_HPP_

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
class ListElementFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~ListElementFunction()
	{
	}

	string id()
	{
		return "list.element";
	}
	string usage()
	{
		return "list.element( N list ) : return element at 'N'th position of 'list'";
	}


	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scan(body);

		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return nullptr;
		}

		int n = scan.nextInt();

		if (n < 0)
		{
			cout << "N must be a positive value!" << endl;
			return nullptr;
		}

		if (n == 0)
		{
			cout << "sorry, this is not C language :)" << endl;
			cout << "0 not included, try a number from 1 to the size of the list" << endl;
			return nullptr;
		}

		n--;

		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return nullptr;
		}

		vector<string>* plist = OptFrameList::readList(ldictionary, scan);
		vector<string>  list;
		if(plist)
		{
			list = vector<string>(*plist);
			delete plist;
		}
		//else
		//	return nullptr;

		if (n >= ((int) list.size()))
		{
			cout << "N is too big! " << (n + 1) << " > " << list.size() << endl;
			return nullptr;
		}

		string element = list.at(n);

		return new string(element);
	}
};

}
#endif /* OPTFRAME_ELEMENT_FUNCTION_HPP_ */
