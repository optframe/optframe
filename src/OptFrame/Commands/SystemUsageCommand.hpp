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

#ifndef USAGEMODULE_HPP_
#define USAGEMODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemUsageCommand : public Command<R, ADS, DS>
{
public:
	virtual ~SystemUsageCommand()
	{
	}

	string id()
	{
		return "system.usage";
	}

	string usage()
	{
		return "system.usage module_or_function";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& all_functions, HeuristicFactory<R, ADS, DS>&, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string rest)
	{
		Scanner scanner(rest);

		string command = scanner.next();

		bool notfound = true;

		for(unsigned int i=0;i<all_modules.size();i++)
			if(all_modules[i]->canHandle(command, ""))
			{
				cout << "Usage: " << all_modules[i]->usage() << endl;
				notfound = false;
				break;
			}

		if(notfound)
			for(unsigned int i=0;i<all_functions.size();i++)
				if(command == all_functions[i]->id())
				{
					cout << "Usage: " << all_functions[i]->usage() << endl;
					notfound = false;
					break;
				}

		if(notfound)
		{
			cout << "Command or function '"<<command<<"' not found!"<<endl;
			return false;
		}

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* USAGEMODULE_HPP_ */
