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

#ifndef OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_
#define OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MoveTSP2Opt.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSP2Opt<T, M> >
class NSIteratorTSP2Opt: public NSIterator<vector<T> , M>
{
	typedef vector<T> Route;

private:

	MOVE* m;
	int p1, p2; // position 1 and position 2, respectively
	const Route& r;

public:

	NSIteratorTSP2Opt(const Route& _r) :
		r(_r)
	{
		m = NULL;
	}

	virtual ~NSIteratorTSP2Opt()
	{
	}

	void first()
	{
		if (r.size() >= 2)
		{
			p1 = 0;
			p2 = 2;
			m = new MoveTSP2Opt<T, M> (p1, p2);
		}
		else
			m = NULL;
	}

	void next()
	{
		if (!((p1 == r.size() - 2) && (p2 == r.size())))
		{
			if (p2 != r.size())
			{
				p2++;
			}
			else
			{
				p1++;
				p2 = p1 + 2;
			}

			m = new MoveTSP2Opt<T, M> (p1, p2);
		}
		else
			m = NULL;
	}

	bool isDone()
	{
		return m == NULL;
	}

	Move<Route, M>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqRouteShift. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

#endif /*OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_*/
