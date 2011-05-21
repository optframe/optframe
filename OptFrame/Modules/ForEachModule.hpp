// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef FOREACHMODULE_HPP_
#define FOREACHMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class ForEachModule: public OptFrameModule<R, M>
{
private:
	OptFrameModule<R, M>* getModule(vector<OptFrameModule<R, M>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	bool exec_modules(vector<OptFrameModule<R, M>*> all_modules, HeuristicFactory<R, M>* factory, map<string, string>* dictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, M>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string rest = m->preprocess(dictionary, scanner.rest());
		m->run(all_modules, factory, dictionary, rest);

		return true;
	}

public:
	string id()
	{
		return "for_each";
	}
	string usage()
	{
		return "for_each var list_of_values list_of_commands";
	}
	void run(vector<OptFrameModule<R, M>*> all_modules, HeuristicFactory<R, M>* factory, map<string, string>* dictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string var = scanner.next();
		vector < string > values;
		vector < string > commands;

		values = HeuristicFactory<R, M>::readList(scanner);
		commands = HeuristicFactory<R, M>::readList(scanner);

		for (int v = 0; v < values.size(); v++)
		{
			(*dictionary)[var] = values.at(v);

			for (int c = 0; c < commands.size(); c++)
			{
				if (!exec_modules(all_modules, factory, dictionary, commands.at(c)))
				{
					cout << "Error in command: " << commands.at(c) << endl;
				}
			}
		}

	}
};

#endif /* FOREACHMODULE_HPP_ */
