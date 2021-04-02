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

#ifndef OPTFRAME_NSITERATOROROPTK_HPP_
#define OPTFRAME_NSITERATOROROPTK_HPP_

// Framework includes
#include "../../../Move.hpp"
#include "../../../NSIterator.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE = MoveTSPSwap<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSIteratorTSPOrOptk: public NSIterator<XES, XEv>
{
	typedef vector<T> Route;

protected:
	int n, k;
	int i, j;

	std::shared_ptr<P> p; // has to be the last

public:

	NSIteratorTSPOrOptk(int _n, int _k, std::shared_ptr<P> _p = nullptr) :
			n(_n), k(_k), p(_p)
	{
		i = j = 0;
	}

	virtual ~NSIteratorTSPOrOptk()
	{
	}

	virtual void first() override
	{
		i = 0;
		j = 1;
	}

	virtual void next() override
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

	virtual bool isDone() override
	{
		return i > n - k;
	}

	virtual uptr<Move<XES, XEv>> current() override
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "OrOpt{K=" << k << "}. Aborting." << endl;
			exit(1);
		}

		return uptr<Move<XES, XEv>>(new MOVE(i, j, k, p));
	}
};

#endif /*OPTFRAME_NSITERATOROROPTK_HPP_*/
