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

#ifndef PAUSEMODULE_HPP_
#define PAUSEMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class PauseModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~PauseModule()
	{
	}

	string id()
	{
		return "pause";
	}

	string usage()
	{
		return "pause";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>&, vector<OptFrameFunction*>&, HeuristicFactory<R, ADS, M>&, map<string, string>&, string)
	{
		cout << "Press <enter> to continue..." << endl;

		getchar();

		return true;
	}

};

#endif /* PAUSEMODULE_HPP_ */
