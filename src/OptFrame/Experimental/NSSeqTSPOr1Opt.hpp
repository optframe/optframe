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


#ifndef OPTFRAME_NSSEQTSP0R1OPT_HPP_
#define OPTFRAME_NSSEQTSP0R1OPT_HPP_

#include "./Moves/MoveTSPOr1Opt.hpp"
#include "./NSIterators/IteratorNSSeqTSPOr1Opt.hpp"

// Framework includes
#include "../NSSeq.hpp"

using namespace std;

/*
 Classical Problem: Traveling Salesman Problem

 The Neighborhood Structure Or-Opt has proposed by ??

 Or-Opt is aplied for any problem that representation is like a vector<T>, where T is the type of the vector.

 e.g: vector<T> where type of T is int
 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSPOr1Opt(2,5) in s

 s': 1 2 4 5 3 6 7 8

 s" after apply MoveTSP2Opt(3,8) in s'

 s": 1 2 4 3 6 7 5 8

 */


template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class NSSeqTSPOr1Opt: public NSSeq<vector<T> , DS >
{
private:
	typedef vector<T> Route;

public:

	NSSeqTSPOr1Opt()
	{
	}

	virtual ~NSSeqTSPOr1Opt()
	{
	}

	using NSSeq<Route, DS >::move;
	using NSSeq<Route, DS >::getIterator;

	Move<Route, DS >& move(const Route& rep)
	{
		if (rep.size() < 2)
			return *new MoveTSPOr1Opt<T, DS > (-1, -1);

		int c = rand() % rep.size();

		int pos = rand() % (rep.size() + 1);

		return *new MoveTSPOr1Opt<T, DS > (c, pos);
	}

	virtual NSIterator<Route, DS >& getIterator(const Route& r)
	{
		return *new IteratorNSSeqTSPOr1Opt<T, DS > (r);
	}

	virtual void print()
	{
		cout << "NSSeqTSPOr1Opt" << endl;
	}
};

#endif /*OPTFRAME_NSSEQTSP0R1OPT_HPP_*/
