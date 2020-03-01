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

#ifndef OPTFRAME_ITERATORTSPSWAP_HPP_
#define OPTFRAME_ITERATORTSPSWAP_HPP_

// Framework includes
#include "../../../Move.hpp"
#include "../../../NSIterator.hpp"

#include "../Moves/MoveTSPSwap.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE = MoveTSPSwap<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>>
class NSIteratorTSPSwap: public NSIterator<S, XEv>
{
	typedef vector<T> Route;

protected:
	//MOVE* m;
   uptr<MOVE> m;
	int p1, p2; // position 1 and position 2, respectively
	int n;

	P* p; // has to be the last

public:

	NSIteratorTSPSwap(int _n, P* _p = nullptr) :
			p(_p)
	{
		p1 = p2 = 0;
		n = _n;
		m = nullptr;
	}

	virtual ~NSIteratorTSPSwap()
	{
	}

	virtual void first() override
	{
		if (n >= 2)
		{
			p1 = 0;
			p2 = 1;
			m = uptr<MOVE>(new MOVE(p1, p2, p));
		}
		else
			m = nullptr;
	}

	virtual void next() override
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

			m = uptr<MOVE>(new MOVE(p1, p2, p));
		}
		else
			m = nullptr;
	}

	virtual bool isDone() override
	{
		return (m == nullptr);
	}

	virtual uptr<Move<S, XEv>> current() override
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqTSPSwap. Aborting." << endl;
			exit(1);
		}

      // steal from 'm'
      uptr<Move<S, XEv>> m2 = std::move(m);
      m = nullptr;

		return m2;
	}
};

#endif /*OPTFRAME_ITERATORTSPSWAP_HPP_*/
