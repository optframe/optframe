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

template<class R, class M>
class UsageModule : public OptFrameModule<R,M>
{
public:
	string id()
	{
		return "usage";
	}
	string usage()
	{
		return "usage module_name";
	}

	void run(vector<OptFrameModule<R,M>*>& all_modules, HeuristicFactory<R,M>*, map<string,string>* dictionary, string rest)
	{
		Scanner scanner(rest);

		string command = scanner.next();

		bool notfound = true;

		for(unsigned int i=0;i<all_modules.size();i++)
			if(command == all_modules[i]->id())
			{
				cout << "Usage: " << all_modules[i]->usage() << endl;
				notfound = false;
				break;
			}

		if(notfound)
			cout << "Module '"<<command<<"' not found!"<<endl;
	}

};

#endif /* USAGEMODULE_HPP_ */
