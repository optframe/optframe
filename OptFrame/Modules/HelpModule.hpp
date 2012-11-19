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

#ifndef HELPMODULE_HPP_
#define HELPMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class HelpModule : public OptFrameModule<R, ADS, M>
{
public:

	virtual ~HelpModule()
	{
	}

	string id()
	{
		return "help";
	}
	string usage()
	{
		return "help";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>*, map<string,string>* dictionary, string)
	{
		cout << "Available modules are:" << endl;
		for(unsigned int i=0;i<all_modules.size();i++)
			cout << all_modules[i]->id() << endl;
		cout << "Type 'usage module_name' to learn how to use the module." << endl;
	}

};

#endif /* HELPMODULE_HPP_ */
