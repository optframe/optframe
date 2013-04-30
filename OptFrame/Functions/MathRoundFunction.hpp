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

#ifndef OPTFRAME_MATH_ROUND_FUNCTION_HPP_
#define OPTFRAME_MATH_ROUND_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

class MathRoundFunction : public OptFrameFunction
{
public:

	virtual ~MathRoundFunction()
	{
	}

	virtual string id()
	{
		return "math.round";
	}

	virtual string usage()
	{
		return "math.round( value ) : return rounded integer value";
	}

	virtual string roundNumber(double v)
	{
		int iv;
		if (v > 0)
			iv = floor(v + 0.5);
		else
			iv = ceil(v - 0.5);
		stringstream ss;
		ss << iv;

		return ss.str();
	}

	virtual string* run(vector<OptFrameFunction*>& allFunctions, map< string, string >&, map< string,vector<string> >&, string body)
	{
		//cout << "abs: '" << body << "'" << endl;
		Scanner scanner(body);

		if(!scanner.hasNext())
			return NULL;

		double v;
		string a = scanner.next();

		try
		{
			v = Scanner::parseDouble(a);
		}
		catch(ConversionError& e)
		{
			cout << id() << " function error: not a number! ('" << a << "')" << endl;
			return NULL;
		}

		return new string(roundNumber(v));
	}
};

#endif /* OPTFRAME_MATH_ROUND_FUNCTION_HPP_ */