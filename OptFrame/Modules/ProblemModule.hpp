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

#ifndef PROBLEMMODULE_HPP_
#define PROBLEMMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ProblemModule: public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~ProblemModule()
	{
	}

	virtual string id()
	{
		return "problem."; // leave dot to implement next module in heritage
	}

	virtual string usage()
	{
		string usage = id();
		usage.append(" [load instance_path | unload]");
		return usage;
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);
		if(!scanner.hasNext())
		{
			cout << "USAGE: " << usage() << endl;
			return false;
		}

		string mode = scanner.next();
		if(mode == "load")
			return load(Scanner::trim(scanner.rest()), factory, dictionary, ldictionary);

		if(mode == "unload")
			return unload(factory, dictionary, ldictionary);

		cout << "problem module: unknown mode '" << mode << "'! (should be 'load instance_path' or 'unload')";

		return false;
	}

	virtual bool load(string filename, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary) = 0;
	virtual bool unload(HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary) = 0;
};

#endif /* PROBLEMMODULE_HPP_ */
