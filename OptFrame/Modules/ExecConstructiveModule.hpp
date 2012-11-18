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

#ifndef EXEC_CONSTRUCTIVE_MODULE_HPP_
#define EXEC_CONSTRUCTIVE_MODULE_HPP_

#include "../OptFrameModule.hpp"
#include "../Constructive.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ExecConstructiveModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ExecConstructiveModule()
	{
	}

	string id()
	{
		return "exec_constructive";
	}

	string usage()
	{
		return "exec OptFrame:Constructive [output_solution_name]";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		cout << "exec_constructive: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		Constructive<R, ADS>* cons;
		factory->assign(cons, scanner.nextInt(), scanner.next()); // reads backwards!

		Solution<R, ADS>& sFinal = cons->generateSolution();


		string s_new_id = "";

		int new_id = factory->addComponent(sFinal);

		stringstream str;
		str << Solution<R, ADS>::idComponent() << " " << new_id;
		s_new_id = str.str();

		cout << "'" << s_new_id << "' added." << endl;

		if (scanner.hasNext())
		{
			string new_name = scanner.next();
			OptFrameModule<R, ADS, M>::run_module("define", all_modules, factory, dictionary, new_name + " " + s_new_id);
		}

	}

};

#endif /* EXEC_CONSTRUCTIVE_MODULE_HPP_ */
