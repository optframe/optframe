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

#ifndef OPTFRAME_MATH_TRUNC_FUNCTION_HPP_
#define OPTFRAME_MATH_TRUNC_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../PreprocessFunction.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MathTruncFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~MathTruncFunction()
	{
	}

	virtual string id()
	{
		return "math.trunc";
	}

	virtual string usage()
	{
		return "math.trunc( value ) : return truncated integer value";
	}

	virtual string truncNumber(double v)
	{
		int iv = v;
		stringstream ss;
		ss << fixed; // disable scientific notation
		ss << iv;
		return ss.str();
	}

	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		//cout << "abs: '" << body << "'" << endl;
		Scanner scanner(body);

		if(!scanner.hasNext())
			return nullptr;

		double v;
		string a = scanner.next();

		try
		{
			v = Scanner::parseDouble(a);
		}
		catch(ConversionError& e)
		{
			cout << id() << " function error: not a number! ('" << a << "')" << endl;
			return nullptr;
		}

		return new string(truncNumber(v));
	}
};

}

#endif /* OPTFRAME_MATH_TRUNC_FUNCTION_HPP_ */
