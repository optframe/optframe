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
		return "next( text ) : return pair [ next , rest ]";
	}

	virtual string formatNumber(double v)
	{
		stringstream ss;
		ss << v;
		return ss.str();
	}

	virtual pair<string, string>* run(vector<OptFrameFunction*>& allFunctions, map< string, string >&, map< string,vector<string> >& ldictionary, string body)
	{
		//cout << "next function run: '" << body << "'" << endl;

		// Maybe in future
		// To be more useful: add spaces before and after '[', ']', '(' , ')' and ','

		//int count_brackets1 = 0; //'('
		//int count_brackets2 = 0; //'['

		stringstream ss;
		unsigned j=0;
		for(unsigned i=0; i<body.length(); i++)
		{
			//if((body.at(i) == ')') && (count_brackets1==0) && (count_brackets2==0))
			if(body.at(i) == ')')
				break;

			/*
			if(body.at(i) == '(')
			{
				count_brackets1++;
				ss << " " << body.at(i) << " ";
			}
			else if(body.at(i) == ')')
			{
				count_brackets1--;
				ss << " " << body.at(i) << " ";
			}
			else if(body.at(i) == '[')
			{
				count_brackets2++;
				ss << " " << body.at(i) << " ";
			}
			else if(body.at(i) == ']')
			{
				count_brackets2--;
				ss << " " << body.at(i) << " ";
			}
			else if(body.at(i) == ',')
			{
				ss << " " << body.at(i) << " ";
			}
			else
				ss << body.at(i);
				*/

			ss << body.at(i);

			j++;
		}

		j++;// drop ')'

		stringstream ssrest;
		for(unsigned i=j; i<body.length(); i++)
			ssrest << body.at(i);

		//cout << "next function run (after filter): '" << ss.str() << "'" << endl;

		Scanner scanner(ss.str());

		/*
		// NO MORE OPTIONS, NOT USEFUL!
		string option = scanner.next();
		string next = "";
		if(option=="word")
			next = scanner.next();
		else if(option=="number")// number
			next = formatNumber(scanner.nextDouble());
		else
		{
			cout << "next function: no such option '" << option << "'" << endl;
			return NULL;
		}
		*/

		string next = scanner.next();
		string rest = scanner.rest();

		stringstream ssr;
		ssr << "[ " << next << " , " << rest << " ]";

		return new pair<string, string>(ssr.str(), ssrest.str());
	}
};

#endif /* OPTFRAME_NEXT_FUNCTION_HPP_ */
