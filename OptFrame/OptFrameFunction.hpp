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

/*
 * OptFrameFunction.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_FUNCTION_HPP_
#define OPTFRAME_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "Scanner++/Scanner.h"

#include <algorithm>

class OptFrameFunction
{
protected:
	pair<string, string> run_function(string func, vector<OptFrameFunction*>& allFunctions, string input)
	{
		for(unsigned int i=0;i<allFunctions.size();i++)
			if(func==allFunctions[i]->id())
			{
				pair<string, string> p = allFunctions[i]->run(allFunctions, input);
				return p;
			}
		cout << "Function '" << func << "' not found." << endl;
		return make_pair("", input);
	}

public:

	virtual ~OptFrameFunction()
	{
	}

	virtual string id() = 0;
	virtual string usage() = 0;

	virtual pair<string, string> run(vector<OptFrameFunction*>& allFunctions, string body) = 0;
};

#endif /* OPTFRAME_FUNCTION_HPP_ */
