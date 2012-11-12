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

#ifndef ELEMENTMODULE_HPP_
#define ELEMENTMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ElementModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ElementModule()
	{
	}

	string id()
	{
		return "element";
	}
	string usage()
	{
		return "element N list new_element_name";
	}

	void run(vector<OptFrameModule<R, ADS, M>*>& all_modules, HeuristicFactory<R, ADS, M>* hf, map<string, string>* dictionary, string input)
	{
		Scanner scan(input);
		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		int n = scan.nextInt();

		if (n < 0)
		{
			cout << "N must be a positive value!" << endl;
			return;
		}

		if (n == 0)
		{
			cout << "sorry, this is not C language :)" << endl;
			cout << "0 not included, try a number from 1 to the size of the list" << endl;
			return;
		}

		n--;

		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		vector < string >& list = hf->readList(scan);

		if (n >= ((int) list.size()))
		{
			cout << "N is too big! " << (n + 1) << " > " << list.size() << endl;
			return;
		}

		if (!scan.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string element = scan.next();

		OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, hf, dictionary, element + " " + list.at(n));
		delete &list;
	}

};

#endif /* ELEMENTMODULE_HPP_ */
