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

#ifndef OPTFRAME_NSITERATOROROPTK_HPP_
#define OPTFRAME_NSITERATOROROPTK_HPP_

// Framework includes
#include "../../../NSIterator.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSIteratorTSPOrOptk: public NSIterator<vector<T> , M>
{
	typedef vector<T> Route;

private:
	int n, k;
	int i, j;

public:

	NSIteratorTSPOrOptk(int _n, int _k) :
		n(_n), k(_k)
	{
	}

	virtual ~NSIteratorTSPOrOptk()
	{
	}

	void first()
	{
		i = 0;
		j = 1;
	}

	void next()
	{
		j++;

		if (j == i)
			j++;

		if (j > n - k)
		{
			j = 0;
			i++;
		}
	}

	bool isDone()
	{
		return i > n - k;
	}

	Move<Route, M>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "OrOpt{K=" << k << "}. Aborting." << endl;
			exit(1);
		}

		return *new MoveTSPOrOptk<T, M> (i, j, k);
	}
};

#endif /*OPTFRAME_NSITERATOROROPTK_HPP_*/
