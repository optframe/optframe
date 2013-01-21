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

#ifndef OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_
#define OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_

#include "../OptFrameModule.hpp"

#include "SystemUnsafeDefineModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemPreprocessModule: public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~SystemPreprocessModule()
	{
	}

	string id()
	{
		return "system.preprocess";
	}

	string usage()
	{
		return "system.preprocess return_value module_name input";
	}

	OptFrameModule<R, ADS, DS>* getModule(vector<OptFrameModule<R, ADS, DS>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string name = scanner.next();

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string module = scanner.next();

		string inp = scanner.rest();

		OptFrameModule<R, ADS, DS>* m = getModule(all_modules, module);

		if(!m)
		{
			cout << "preprocess module: NULL module!" << endl;
			return false;
		}

		string* final = m->preprocess(allFunctions,dictionary,ldictionary, inp);

		if(!final)
			return false;

		stringstream ss;
		ss << name << " " << (*final);

		delete final;

		return OptFrameModule<R, ADS, DS>::run_module("system.unsafe_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
	}

	// runs raw module without preprocessing
	virtual string* preprocess(vector<OptFrameFunction*>&, map<string, string>&, map< string,vector<string> >&, string input)
	{
		return new string(input); // disable pre-processing
	}

};

#endif /* OPTFRAME_SYSTEM_PREPROCESS_MODULE_HPP_ */
