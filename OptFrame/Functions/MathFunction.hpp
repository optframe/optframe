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

#ifndef OPTFRAME_MATH_FUNCTION_HPP_
#define OPTFRAME_MATH_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class MathFunction : public OptFrameFunction
{
public:

	virtual ~MathFunction()
	{
	}

	virtual string id()
	{
		return "math";
	}

	virtual string usage()
	{
		return "math( A operation B ) : return operation of numbers A and B according to operation (+, -, *, /)";
	}

	virtual string formatNumber(double v)
	{
		stringstream ss;
		ss << v;
		return ss.str();
	}

	virtual pair<string, string>* run(vector<OptFrameFunction*>&, map< string,vector<string> >&, string body)
	{
		Scanner scanner(body);

		double a  = scanner.nextDouble();
		string op = scanner.next();
		double b  = scanner.nextDouble();

		scanner.next(); // drop ')'

			if(op == "+")
				return new pair<string, string>(formatNumber(a+b), scanner.rest());
			if(op == "-")
				return new pair<string, string>(formatNumber(a-b), scanner.rest());
			if(op == "*")
				return new pair<string, string>(formatNumber(a*b), scanner.rest());
			if(op == "/")
				return new pair<string, string>(formatNumber(a/b), scanner.rest());

		cout << "math function: no such operation '" << op << "'" << endl;

		return NULL;
	}
};

#endif /* OPTFRAME_MATH_FUNCTION_HPP_ */
