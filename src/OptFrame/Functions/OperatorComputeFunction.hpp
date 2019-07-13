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

#include "../PreprocessFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class OperatorComputeFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~OperatorComputeFunction()
	{
	}

	virtual string id()
	{
		return "operator.compute";
	}

	virtual string usage()
	{
		return "operator.compute( A operation B ) : return operation of numbers A and B according to operation (+, -, *, /)";
	}

	virtual string formatNumber(double v)
	{
		stringstream ss;
		//ss.precision(PreprocessFunction<R,ADS,DS>::precision); // set float precision
		ss << fixed; // disable scientific notation
		ss << v;
		return ss.str();
	}

	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		if(!scanner.hasNext())
			return nullptr;

		string sa = scanner.next();

		if(!scanner.hasNext())
			return nullptr;

		string op = scanner.next();

		if(!scanner.hasNext())
			return nullptr;

		string sb = scanner.next();

		double a;
		double b;

		try
		{
			a = Scanner::parseDouble(sa);
			b = Scanner::parseDouble(sb);
		}
		catch(ConversionError& e)
		{
			cout << "math function: not a number to operate ('" <<sa << "' " << op << " '" << sb << "')!" << endl;
			return nullptr;
		}

		if(op == "+")
			return new string(formatNumber(a+b));
		if(op == "-")
			return new string(formatNumber(a-b));
		if(op == "*")
			return new string(formatNumber(a*b));
		if(op == "/")
			return new string(formatNumber(a/b));
		if(op == "mod")
			return new string(formatNumber(((int)a)%((int)b)));

		cout << "math function: no such operation '" << op << "'" << endl;

		return nullptr;
	}
};

}

#endif /* OPTFRAME_MATH_FUNCTION_HPP_ */
