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

#ifndef OPTFRAME_COMPARE_FUNCTION_HPP_
#define OPTFRAME_COMPARE_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class CompareFunction : public OptFrameFunction
{
public:

	virtual ~CompareFunction()
	{
	}

	virtual string id()
	{
		return "compare";
	}

	virtual string usage()
	{
		return "compare( A signal B ) : return comparison of numbers A and B according to signal (==, !=, >, >=, <, <=)";
	}

	string formatBool(bool b)
	{
		if (b)
			return "true";
		else
			return "false";
	}

	virtual string* run(vector<OptFrameFunction*>& allFunctions, map< string, string >&, map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		string sa     = scanner.next();
		string signal = scanner.next();
		string sb     = scanner.next();

		if((signal == "==") || (signal == "!=")) // compare as string
		{
			if(signal == "==")
				return new string(formatBool(sa == sb));
			if(signal == "!=")
				return new string(formatBool(sa != sb));
		}

		if((signal == ">") || (signal == ">=") || (signal == "<") || (signal == "<=")) // compare as number
		{
			double a;
			double b;

			try
			{
				a = Scanner::parseDouble(sa);
				b = Scanner::parseDouble(sb);
			}
			catch(ConversionError& e)
			{
				cout << "compare function: not a number to compare ('" <<sa << "' " << signal << " '" << sb << "')!" << endl;
				return NULL;
			}

			if(signal == ">")
				return new string(formatBool(a>b));
			if(signal == "<")
				return new string(formatBool(a<b));
			if(signal == ">=")
				return new string(formatBool(a>=b));
			if(signal == "<=")
				return new string(formatBool(a<=b));
		}

		cout << "compare function: no such comparison signal '" << signal << "'" << endl;

		return NULL;
	}
};

#endif /* OPTFRAME_COMPARE_FUNCTION_HPP_ */
