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

#ifndef PRINTMODULE_HPP_
#define PRINTMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class PrintModule : public OptFrameModule<R,M>
{
public:
	string id()
	{
		return "print";
	}
	string usage()
	{
		string u = "print loadsol id";

		return u;
	}

	void run(vector<OptFrameModule<R,M>*>& all_modules, HeuristicFactory<R,M>* factory, map<string,string>* dictionary, string input)
	{
		cout << "print: "<<input<<endl;
		Scanner scanner(input);

		if(!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string sol = scanner.next();

		if(sol!="loadsol")
		{
			cout << "First parameter must be a 'loadsol'!" << endl;
			cout << "Usage: "<<usage()<<endl;
			return;
		}

		string id = scanner.next();

		Scanner s2(sol+" "+id);
		Solution<R>* s = factory->read_loadsol(&s2);
		s->print();
	}

};

#endif /* PRINTMODULE_HPP_ */
