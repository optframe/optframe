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

#ifndef OPTFRAME_SHAPIRO_TEST_FUNCTION_HPP_
#define OPTFRAME_SHAPIRO_TEST_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class ShapiroTestFunction : public OptFrameFunction
{
public:

	virtual ~ShapiroTestFunction()
	{
	}

	virtual string id()
	{
		return "shapiro_test";
	}

	virtual string usage()
	{
		return "shapiro_test( list ) : return p-value\nnull hypothesis: values are normal, if p-value < alpha reject null hypothesis.";
	}

	virtual string formatNumber(double v)
	{
		stringstream ss;
		ss << v;
		return ss.str();
	}

	virtual pair<string, string>* run(vector<OptFrameFunction*>& allFunctions, map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		vector<string>* plist = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list;
		if(plist)
		{
			list = vector<string>(*plist);
			delete plist;
		}
		else
			return NULL;

		if(list.size()==0)
			return NULL;

		stringstream scommand;
		scommand << "echo \"x <- c(";

		for(unsigned i=0; i<list.size(); i++)
		{
			scommand << list.at(i);
			if(i != list.size()-1)
				scommand << ",";
		}

		scommand << ") \n shapiro.test(x)\" | R --no-save | grep p-value";

		FILE* pPipe = popen(scommand.str().c_str(), "r");
		if (pPipe == NULL)
		{
		    cout << "shapiro_test module: PIPE NOT OPEN!" << endl;
		    return NULL;
		}

		char line[100];

		string output = "";

		while(fgets(line, 100, pPipe) != NULL)
		{
			string sline = line;
			output.append(sline);
		}

		pclose(pPipe);

		//cout << "shapiro_test module: OUTPUT '" << output << "'" << endl;
		if(output=="") // POSSIBLE ERROR: 'sh: R: not found'
			return NULL;

		Scanner scan_out(output); //example: 'W = 0.9819, p-value = 0.9606'
		scan_out.next(); // drop 'W'
		scan_out.next(); // drop '='
		scan_out.next(); // drop '0.9819,'
		scan_out.next(); // drop 'p-value'
		scan_out.next(); // drop '='
		double pvalue = scan_out.nextDouble();

		scanner.next(); // drop ')'

		return new pair<string, string>(formatNumber(pvalue), scanner.rest());
	}
};

#endif /* OPTFRAME_SHAPIRO_TEST_FUNCTION_HPP_ */
