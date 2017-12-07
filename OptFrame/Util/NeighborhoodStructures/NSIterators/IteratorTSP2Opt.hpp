// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_
#define OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_

// Framework includes
#include "../../../NSIterator.hpp"

#include "../Moves/MoveTSP2Opt.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveTSP2Opt<T>, class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorTSP2Opt: public NSIterator<vector<T>, ADS>
{
	typedef vector<T> Route;

protected:
	MOVE* m;

	int p1, p2; // position 1 and position 2, respectively
	const Route& r;

	P* p; // has to be the last

public:

	NSIteratorTSP2Opt(const Route& _r, P* _p = nullptr) :
			r(_r), p(_p)
	{
		p1 = 0; // avoid non-initialization warning
		p2 = 2; // avoid non-initialization warning

		m = nullptr;
	}

	virtual ~NSIteratorTSP2Opt()
	{
	}

	virtual void first()
	{
		if (r.size() >= 2)
		{
			p1 = 0;
			p2 = 2;
			m = new MOVE(p1, p2, p);
		}
		else
			m = nullptr;
	}

	virtual void next()
	{
		if (!((p1 == ((int) r.size()) - 2) && (p2 == ((int) r.size()))))
		{
			if (p2 != ((int) r.size()))
			{
				p2++;
			}
			else
			{
				p1++;
				p2 = p1 + 2;
			}

			m = new MOVE(p1, p2, p);
		}
		else
			m = nullptr;
	}

	virtual bool isDone()
	{
		return m == nullptr;
	}

	virtual Move<Route, ADS>& current()
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
