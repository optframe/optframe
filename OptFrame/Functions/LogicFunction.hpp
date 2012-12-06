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

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class LogicFunction : public OptFrameFunction
{
public:

	virtual ~LogicFunction()
	{
	}

	virtual string id()
	{
		return "logic";
	}

	virtual string usage()
	{
		return "logic( [ A operation B | not A ] ) : return operation of numbers A and B according to operation (and, or)";
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

	virtual pair<string, string>* run(vector<OptFrameFunction*>&, map< string, string >&, map< string,vector<string> >&, string body)
	{
		Scanner scanner(body);

		string first = scanner.next();

		if(first == "not")
		{
			bool a  = parseBool(scanner.next());

			scanner.next(); // drop ')'

			return new pair<string, string>(formatBool(!a), scanner.rest());
		}
		else
		{
			bool a  = parseBool(first);
			string op = scanner.next();
			bool b  = parseBool(scanner.next());

			scanner.next(); // drop ')'

			if(op=="and")
				return new pair<string, string>(formatBool(a && b), scanner.rest());

			if(op=="or")
				return new pair<string, string>(formatBool(a || b), scanner.rest());

			cout << "logic function: no such operation '" << op << "'" << endl;

			return NULL;
		}
	}
};

#endif /* OPTFRAME_LOGIC_FUNCTION_HPP_ */
