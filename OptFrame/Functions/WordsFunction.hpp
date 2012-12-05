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

#ifndef OPTFRAME_WORDS_FUNCTION_HPP_
#define OPTFRAME_WORDS_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class WordsFunction : public OptFrameFunction
{
public:

	virtual ~WordsFunction()
	{
	}

	virtual string id()
	{
		return "words";
	}

	virtual string usage()
	{
		return "next( string ) : return list_of_words";
	}

	virtual pair<string, string>* run(vector<OptFrameFunction*>&, map< string,vector<string> >&, string body)
	{
		Scanner scanner(body);

		stringstream ssr;
		ssr << "[ ";
		string next = scanner.next();
		if(next != ")")
			ssr << " " << next << " ";
		else
			return NULL; // EMPTY

		next = scanner.next();
		while(next != ")")
		{
			ssr << ", " << next << " ";
			next = scanner.next();
		}

		ssr << " ]";

		return new pair<string, string>(ssr.str(), scanner.rest());
	}
};

#endif /* OPTFRAME_WORDS_FUNCTION_HPP_ */
