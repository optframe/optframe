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

#ifndef OPTFRAME_NSSEQTSP2OPT_HPP_
#define OPTFRAME_NSSEQTSP2OPT_HPP_

/*
 Classical Problem: Traveling Salesman Problem

 The Neighborhood Structure 2-opt has proposed by Cross (1958)
 Croes G., A method for solving traveling salesman problems. Operations Research 6 (1958), pp. 791–812

 2-Opt is aplied for any problem that representation is like a vector<T>, where T is the type of the vector.

 e.g: vector<T> where type of T is int
 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSP2Opt(2,5) in s

 s': 1 2 5 4 3 6 7 8

 s" after apply MoveTSP2Opt(3,8) in s'

 s": 1 2 5 8 7 6 3 4

 */

// Framework includes
#include "../../Move.hpp"
#include "../../NSSeq.hpp"

#include "Moves/MoveTSP2Opt.hpp"
#include "NSIterators/IteratorTSP2Opt.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSP2Opt<T, M> >
class NSSeqTSP2Opt: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;

private:

public:

	NSSeqTSP2Opt()
	{
	}

	virtual ~NSSeqTSP2Opt()
	{
	}

	Move<Route, M>& move(const Route& rep)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1);

		int p1 = rand() % (rep.size() + 1);

		int p2;

		do
		{
			p2 = rand() % (rep.size() + 1);
		} while (abs(p1 - p2) < 2);

		// create 2-opt(p1,p2) move
		return *new MOVE(p1, p2);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& r)
	{
		return *new NSIteratorTSP2Opt<T, M, MOVE> (r);
	}

	virtual void print()
	{
		cout << "NSSeqTSP2Opt" << endl;
	}
};

#endif /*OPTFRAME_NSSEQTSP2OPT_HPP_*/
