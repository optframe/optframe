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

#ifndef OPTFRAME_ACTION_HPP_
#define OPTFRAME_ACTION_HPP_

#include <cstdlib>
#include <iostream>
#include <vector>

#include "Scanner++/Scanner.h"

//#include "Module.hpp"

using namespace std;
using namespace scannerpp;

template<class R, class ADS, class DS> class HeuristicFactory;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Action
{
public:

	virtual ~Action()
	{
	}

	virtual string usage() = 0;

	virtual bool handleComponent(string component) = 0;

	virtual bool handleAction(string action) = 0;

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary) = 0;

	static bool addAndRegister(Scanner& scanner, OptFrameComponent& comp, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		int index = hf.addComponent(comp);

		if (index == -1)
			return false;

		if (scanner.hasNext())
		{
			string varName = scanner.next();

			stringstream sscomp;
			sscomp << comp.id() << " " << index;

			d[varName] = sscomp.str(); // TODO: fix!!

			return true;
			//return Module<R, ADS, DS>::defineText(varName, sscomp.str(), d);
		}

		return true;
	}

};

}

#endif /* OPTFRAME_ACTION_HPP_ */
