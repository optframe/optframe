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

#ifndef OPTFRAME_SCOMPARE_FUNCTION_HPP_
#define OPTFRAME_SCOMPARE_FUNCTION_HPP_

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
class OperatorSCompareFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~OperatorSCompareFunction()
	{
	}

	virtual string id()
	{
		return "operator.scompare";
	}

	virtual string usage()
	{
		return "operator.scompare( A signal B ) : return comparison of numbers A and B according to signal (==, !=, >, >=, <, <=)";
	}

	string formatBool(bool b)
	{
		if (b)
			return "true";
		else
			return "false";
	}

	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		//cout << "compare: '" << body << "'" << endl;
		Scanner scanner(body);
		scanner.useSeparators("=+><!");

		if(!scanner.hasNext())
			return nullptr;

		string sa     = Scanner::trim(scanner.next());
		//cout << "SA: " << sa << endl;

		scanner.useDefaultSeparators();
		if(!scanner.hasNext())
			return nullptr;

		string signal = scanner.next();
		//cout << "OP: " << signal << endl;

		if(!scanner.hasNext())
			return nullptr;

		string sb     = Scanner::trim(scanner.rest());
		//cout << "SB: " << sb << endl;

		if((signal == "==") || (signal == "!=")) // compare as string
		{
			if(signal == "==")
				return new string(formatBool(sa == sb));
			if(signal == "!=")
				return new string(formatBool(sa != sb));
		}

		cout << "scompare function: no such comparison '" << sa << "' '" << signal << "' '" << sb << "'" << endl;

		return nullptr;
	}
};

}

#endif /* OPTFRAME_COMPARE_FUNCTION_HPP_ */
