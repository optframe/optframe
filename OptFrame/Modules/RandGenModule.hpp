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

#ifndef RANDGENMODULE_HPP_
#define RANDGENMODULE_HPP_

#include "../OptFrameModule.hpp"
#include "../RandGen.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class RandGenModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~RandGenModule()
	{
	}

	string id()
	{
		return "randgen";
	}

	string usage()
	{
		return "randgen system_seed seed\n Where: 'seed' is a positive integer value for the system random number generator.";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string ss = scanner.next(); // drop system_seed option

		if (ss != "system_seed")
		{
			cout << "invalid option: '" << ss << "'" << endl;
			cout << "Usage: " << usage() << endl;
			return;
		}

		if (!scanner.hasNext())
		{
			cout << "missing 'seed' positive integer value!" << endl;
			cout << "Usage: " << usage() << endl;
			return;
		}

		string strseed = scanner.next();

		unsigned long seed;

		if (strseed == "time()")
		{
			seed = time(NULL);
		}
		else
		{
			Scanner scanseed(strseed);
			seed = scanseed.nextInt();
		}

		cout << "randgen module: setting system random number generator seed to: " << seed << endl;

		RandGen& rg = factory->getRandGen();
		rg.setSeed(seed);
		rg.initialize();
	}

};

#endif /* RANDGENMODULE_HPP_ */
