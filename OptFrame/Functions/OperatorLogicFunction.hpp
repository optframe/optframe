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

#ifndef OPTFRAME_LOGIC_FUNCTION_HPP_
#define OPTFRAME_LOGIC_FUNCTION_HPP_

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
class OperatorLogicFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~OperatorLogicFunction()
	{
	}

	virtual string id()
	{
		return "operator.logic";
	}

	virtual string usage()
	{
		return "operator.logic( [ A operation B | not A ] ) : return operation of numbers A and B according to operation (and, or)";
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

	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		if(!scanner.hasNext())
			return nullptr;

		string first = scanner.next();

		if(first == "not")
		{
			bool a  = parseBool(scanner.next());

			return new string(formatBool(!a));
		}
		else
		{
			bool a  = parseBool(first);
			string op = scanner.next();
			bool b  = parseBool(scanner.next());

			if(op=="and")
				return new string(formatBool(a && b));

			if(op=="or")
				return new string(formatBool(a || b));

			cout << "logic function: no such operation '" << op << "'" << endl;

			return nullptr;
		}
	}
};

}

#endif /* OPTFRAME_LOGIC_FUNCTION_HPP_ */
