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

#ifndef OPTFRAME_OPERATOR_IN_FUNCTION_HPP_
#define OPTFRAME_OPERATOR_IN_FUNCTION_HPP_

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
class OperatorInFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~OperatorInFunction()
	{
	}

	virtual string id()
	{
		return "operator.in";
	}

	virtual string usage()
	{
		return "operator.in( list value ) : return true if value is inside list, false otherwise";
	}

	string formatBool(bool b)
	{
		if(b)
			return "true";
		else
			return "false";
	}

	bool parseBool(string b)
	{
		return b == "true";
	}

	bool in(string s, vector<string>& vs)
	{
		for(unsigned i=0; i<vs.size(); i++)
			if(vs[i] == s)
				return true;
		return false;
	}

	virtual string* run(std::vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		if(!scanner.hasNext())
			return nullptr;

		vector<string>* plist = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list;
		if(plist)
		{
			list = vector<string>(*plist);
			delete plist;
		}
		else
		{
			cout << "function in: '" << body << "'" << std::endl;
			return nullptr;
		}

		if(!scanner.hasNext())
			return nullptr;

		string value = Scanner::trim(scanner.rest());

		return new string(formatBool(in(value, list)));
	}
};

}

#endif /* OPTFRAME_OPERATOR_IN_FUNCTION_HPP_ */
