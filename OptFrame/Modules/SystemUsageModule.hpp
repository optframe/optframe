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

#ifndef USAGEMODULE_HPP_
#define USAGEMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemUsageModule : public OptFrameModule<R, ADS, DS>
{
public:
	virtual ~SystemUsageModule()
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

	bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& all_functions, HeuristicFactory<R, ADS, DS>&, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string rest)
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
			cout << "Module or function '"<<command<<"' not found!"<<endl;
			return false;
		}

		return true;
	}

};

#endif /* USAGEMODULE_HPP_ */
