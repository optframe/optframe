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

#ifndef PRINTMODULE_HPP_
#define PRINTMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class PrintModule: public OptFrameModule<R, ADS, M>
{
public:
	string id()
	{
		return "print";
	}
	string usage()
	{
		string u = "print [ loadsol id |  loadpop id ]  ";

		return u;
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* factory, map<string, string>* dictionary, string input)
	{
		cout << "print: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string sol = scanner.next();

		if (sol != "loadsol" && sol != "loadpop")
		{
			cout << "First parameter must be a 'loadsol' or  a 'loadpop'!" << endl;
			cout << "Usage: " << usage() << endl;
			return;
		}

		if (sol == "loadpop")
		{
			string id = scanner.next();
			Scanner pop(sol + " " + id);
			Population<R, ADS>* p = factory->read_loadpop(&pop);
			p->print();
			return;
		}

		string id = scanner.next();

		Scanner s2(sol + " " + id);
		Solution<R, ADS>* s = factory->read_loadsol(&s2);
		s->print();
	}

};

#endif /* PRINTMODULE_HPP_ */
