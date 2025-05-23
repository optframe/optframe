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

#ifndef OPTFRAME_INPUT_FUNCTION_HPP_
#define OPTFRAME_INPUT_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include <algorithm>

#include "../Scanner++/Scanner.h"
#include "../PreprocessFunction.hpp"
#include "../OptFrameList.hpp"
#include "../HeuristicFactory.hpp"


namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemInputFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~SystemInputFunction()
	{
	}

	virtual string id()
	{
		return "system.input";
	}

	virtual string usage()
	{
		return "system.input() : return user keyboard input";
	}

	virtual string* run(std::vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(&cin);

		string input = Scanner::trim(scanner.nextLine());

		Scanner scan(body);

		return new string(input);
	}
};

}

#endif /* OPTFRAME_INPUT_FUNCTION_HPP_ */
