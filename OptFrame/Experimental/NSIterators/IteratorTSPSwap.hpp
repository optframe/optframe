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

#ifndef OPTFRAME_ITERATORTSPSWAP_HPP_
#define OPTFRAME_ITERATORTSPSWAP_HPP_

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MoveTSPSwap.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSPSwap<T, M> >
class NSIteratorTSPSwap: public NSIterator<vector<T> , M>
{
	typedef vector<T> Route;

private:
	MOVE* m;
	int p1, p2; // position 1 and position 2, respectively
	int n;

public:

	NSIteratorTSPSwap(int _n) :
		n(_n), m(NULL)
	{
	}

	virtual ~NSIteratorTSPSwap()
	{
	}

	void first()
	{
		if (n >= 2)
		{
			p1 = 0;
			p2 = 1;
			m = new MoveTSPSwap<T, M> (p1, p2);
		}
		else
			m = NULL;
	}

	void next()
	{
		if (!((p1 == n - 2) && (p2 == n - 1)))
		{
			if (p2 < (n - 1))
				p2++;

			else
			{
				p1++;
				p2 = p1 + 1;
			}

			m = new MoveTSPSwap<T, M> (p1, p2);
		}
		else
			m = NULL;
	}

	bool isDone()
	{
		return (m == NULL);
	}

	Move<Route, M>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqTSPSwap. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

#endif /*OPTFRAME_ITERATORTSPSWAP_HPP_*/
