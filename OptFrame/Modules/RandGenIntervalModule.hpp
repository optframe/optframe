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

#ifndef RANDOM_NUMBER_INTERVAL_MODULE_HPP_
#define RANDOM_NUMBER_INTERVAL_MODULE_HPP_

#include "../OptFrameModule.hpp"
#include "../RandGen.hpp"

#include "SystemSilentDefineModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandGenIntervalModule: public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~RandGenIntervalModule()
	{
	}

	string id()
	{
		return "randgen.interval";
	}

	string usage()
	{
		return "randgen.interval begin end [stored_number]\n Where: 'begin' and 'end' are positive integer values; 'stored_number' is the randomized number from [begin,end].";
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);
		//cout << "random_number_interval module: '" << input << "'" << endl;

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}


		if (!scanner.hasNext())
		{
			cout << "missing 'begin' positive integer value!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		int begin = scanner.nextInt();

		if (!scanner.hasNext())
		{
			cout << "missing 'end' positive integer value!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		int end = scanner.nextInt();

		RandGen& rg = factory.getRandGen();
		int value = rg.rand(end-begin+1) + begin;

		//cout << "random_number_interval VALUE=" << value << endl;

		if (scanner.hasNext())
		{
			string new_name = scanner.next();
			stringstream ss;
			ss << new_name << " " << value;
			return OptFrameModule<R, ADS, DS>::run_module("system.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
		}
		else
		{
			cout << "random_number_interval module: random number is " << value << endl;
			return true;
		}
	}

};

#endif /* RANDOM_NUMBER_INTERVAL_MODULE_HPP_ */
