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

#ifndef OPTFRAME_CHDIR_COMMAND_HPP_
#define OPTFRAME_CHDIR_COMMAND_HPP_

#include <unistd.h>
#include <stdlib.h>

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemChDirCommand: public Command<R, ADS, DS>
{
public:

	virtual ~SystemChDirCommand()
	{
	}

	string id()
	{
		return "system.chdir";
	}

	string usage()
	{
		return "system.chdir directory";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary, string input)
	{
		string dir = Scanner::trim(input);

		if (dir == "")
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		if (chdir(dir.c_str()) != 0)
		{
			cout << "module " << id() << " error: failed to change directory!" << endl;
			return false;
		}

		char *path = nullptr;
		size_t size = 0;
		path = getcwd(path, size);

		dictionary["optframepath"] = path;

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* OPTFRAME_CHDIR_COMMAND_HPP_ */
