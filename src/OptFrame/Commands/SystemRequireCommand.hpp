// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef OPTFRAME_SYSTEM_REQUIRE_MODULE_HPP_
#define OPTFRAME_SYSTEM_REQUIRE_MODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemRequireCommand : public Command<R, ADS, DS>
{
public:

	virtual ~SystemRequireCommand()
	{
	}

	string id()
	{
		return "system.require";
	}

	string usage()
	{
		return "system.require module_name";
	}


	bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);
		if(!scanner.hasNext()) // no module
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		scanner.useSeparators(" \t\r\n;"); // prevent errors with semicolon

		string module_name = scanner.next();

		for(unsigned i=0; i<allCommands.size(); i++)
			if(allCommands[i]->id() == module_name)
				return true;

		return false;
	}


	// disable preprocess, only need module name
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		// disable preprocess!!
		return new string(input);
	}

};

}

#endif /* OPTFRAME_SYSTEM_REQUIRE_MODULE_HPP_ */
