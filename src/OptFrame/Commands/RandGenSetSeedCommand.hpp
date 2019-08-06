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

#ifndef RANDGENMODULE_HPP_
#define RANDGENMODULE_HPP_

#include "../Command.hpp"
#include "../RandGen.hpp"


namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandGenSetSeedCommand: public Command<R, ADS, DS>
{
public:

	virtual ~RandGenSetSeedCommand()
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

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "missing 'seed' positive integer value!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		unsigned long seed = scanner.nextInt();

		cout << "randgen.set_seed command: setting system random number generator seed to: " << seed << endl;

		RandGen& rg = factory.getRandGen();
		rg.setSeed(seed);
		rg.initialize();

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* RANDGENMODULE_HPP_ */
