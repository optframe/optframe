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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandGenSetSeedModule: public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~RandGenSetSeedModule()
	{
	}

	string id()
	{
		return "randgen.set_seed";
	}

	string usage()
	{
		return "randgen.set_seed seed\n Where: 'seed' is a positive integer value for the system random number generator.";
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "missing 'seed' positive integer value!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		unsigned long seed = scanner.nextInt();

		cout << "randgen.set_seed module: setting system random number generator seed to: " << seed << endl;

		RandGen& rg = factory.getRandGen();
		rg.setSeed(seed);
		rg.initialize();

		return true;
	}

};

#endif /* RANDGENMODULE_HPP_ */
