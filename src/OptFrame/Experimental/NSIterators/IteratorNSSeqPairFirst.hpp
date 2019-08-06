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

#ifndef OPTFRAME_ITERATORNSSEQPAIRFIRST_HPP_
#define OPTFRAME_ITERATORNSSEQPAIRFIRST_HPP_

// Framework includes

#include "../../NSIterator.hpp"
#include "../Moves/MovePairFirst.hpp"

using namespace std;

class PairFirstAdapterIteratorOutOfBound
{
public:
	PairFirstAdapterIteratorOutOfBound()
	{
	}
};

template<class T1, class T2, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MovePairFirst<T1, T2, DS > >
class IteratorNSSeqPairFirst: public NSIterator<pair<T1, T2> , DS >
{
private:
	NSIterator<T1, DS >& iterator;

public:

	IteratorNSSeqPairFirst(NSIterator<T1, DS >& it) :
		iterator(it)
	{
	}

	virtual ~IteratorNSSeqPairFirst()
	{
		delete &iterator;
	}

	void first()
	{
		iterator.first();
	}

	void next()
	{
		iterator.next();
	}

	bool isDone()
	{
		return iterator.isDone();
	}

	Move<pair<T1, T2> , DS >& current()
	{

		if (!iterator.isDone())
			return *new MOVE(iterator.current());
		else
			throw PairFirstAdapterIteratorOutOfBound();
	}
};

#endif /*OPTFRAME_ITERATORNSSEQPAIRFIRST_HPP_*/
