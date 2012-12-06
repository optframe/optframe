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

#ifndef OPTFRAME_RUN_MODULE_HPP_
#define OPTFRAME_RUN_MODULE_HPP_

#include<string>

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class RunModule: public OptFrameModule<R, ADS, M>
{
public:
	OptFrameModule<R, ADS, M>* getModule(vector<OptFrameModule<R, ADS, M>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	bool exec_command(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, ADS, M>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string rest = m->preprocess(allFunctions, dictionary, ldictionary, scanner.rest());
		return m->run(all_modules, allFunctions, factory, dictionary, ldictionary, rest);
	}

	virtual ~RunModule()
	{
	}

	string id()
	{
		return "run";
	}

	string usage()
	{
		return "run list_of_commands";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>  lcommands;
		vector<string>* p_lcommands = OptFrameList::readList(ldictionary, scanner);
		if(p_lcommands)
		{
			lcommands = vector<string>(*p_lcommands);
			delete p_lcommands;
		}
		else
			return false;

		for (unsigned int c = 0; c < lcommands.size(); c++)
		{
			string command = lcommands.at(c);

			if ((command != "") && (command.at(0) == '%')) // first line comment
				command = "";

			if (command != "")
				if (!exec_command(allModules, allFunctions, factory, dictionary, ldictionary, command))
				{
					if (lcommands.at(c) == "")
						cout << "run module: empty command! (perhaps an extra comma in list?)" << endl;
					else
						cout << "run module: error in command '" << lcommands.at(c) << "'" << endl;

					return false;
				}
		}

		return true;
	}

	// leave preprocessing to each module
	virtual string preprocess(vector<OptFrameFunction*>&, map<string, string>&, map< string,vector<string> >&, string input)
	{
		return input; // disable pre-processing
	}
};

#endif /* OPTFRAME_RUN_MODULE_HPP_ */
