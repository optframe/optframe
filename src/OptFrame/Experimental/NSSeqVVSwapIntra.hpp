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

#ifndef OPTFRAME_NSSEQVVSWAPINTRA_HPP_
#define OPTFRAME_NSSEQVVSWAPINTRA_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "Moves/MoveVVSwapIntra.hpp"
#include "NSIterators/IteratorVVSwapIntra.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class NSSeqVVSwapIntra: public NSSeq<vector<vector<T> > , DS >
{
	typedef vector<vector<T> > Routes;

public:

	NSSeqVVSwapIntra()
	{
	}

	virtual ~NSSeqVVSwapIntra()
	{
	}

	Move<Routes, DS >& move(const Routes& rep)
	{
		int i = rand() % rep.size();

		int n = rep.at(i).size();

		int j = -1;

		if (n > 0)
			j = rand() % n;

		int k = j;

		if (n > 1)
			while (k == j)
				k = rand() % n;

		return *new MoveVVSwapIntra<T, DS > (i, j, k);
	}

	virtual NSIterator<Routes, DS >& getIterator(const Routes& r)
	{
		return *new NSIteratorVVSwapIntra<T, DS > (r);
	}

	virtual void print()
	{
		cout << "NSSeqVVSwapIntra" << endl;
	}

};

#endif /*OPTFRAME_NSSEQVVSWAPINTRA_HPP_*/
