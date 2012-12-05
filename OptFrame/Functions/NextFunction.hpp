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

#ifndef OPTFRAME_NEXT_FUNCTION_HPP_
#define OPTFRAME_NEXT_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class NextFunction : public OptFrameFunction
{
public:

	virtual ~NextFunction()
	{
	}

	virtual string id()
	{
		return "next";
	}

	virtual string usage()
	{
		return "next( [word|number] string ) : return pair [ next , rest ]";
	}

	virtual string formatNumber(double v)
	{
		stringstream ss;
		ss << v;
		return ss.str();
	}

	virtual pair<string, string>* run(vector<OptFrameFunction*>& allFunctions, map< string,vector<string> >& ldictionary, string body)
	{
		// replace ',' by espace
		stringstream ss;
		unsigned j=0;
		for(unsigned i=0; i<body.length(); i++)
		{
			if(body.at(i) == ')')
				break;

			if(body.at(i) == ',')
				ss << ' ';
			else
				ss << body.at(i);

			j++;
		}

		j++;// drop ')'

		stringstream ssrest;
		for(unsigned i=j; i<body.length(); i++)
			ssrest << body.at(i);

		Scanner scanner(ss.str());

		string option = scanner.next();
		string next = "";
		if(option=="word")
			next = scanner.next();
		else // number
			next = formatNumber(scanner.nextDouble());

		string rest = scanner.rest();

		stringstream ssr;
		ssr << "[ " << next << " , " << rest << " ]";

		return new pair<string, string>(ssr.str(), ssrest.str());
	}
};

#endif /* OPTFRAME_NEXT_FUNCTION_HPP_ */
