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

#ifndef OPTFRAME_ITERATORNSSEQUNION_HPP_
#define OPTFRAME_ITERATORNSSEQUNION_HPP_

// Framework includes

#include "../../NSIterator.hpp"
#include "../Moves/MoveNSSeqUnion.hpp"

using namespace std;

class NSSeqUnionAdapterOutOfBound
{
public:
	NSSeqUnionAdapterOutOfBound()
	{
	}
};

template<class R, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveNSSeqUnion<R, M> >
class IteratorNSSeqUnion: public NSIterator<R, M>
{
private:
	NSIterator<R, M>& i1;
	NSIterator<R, M>& i2;

public:

	IteratorNSSeqUnion(NSIterator<R, M>& _i1, NSIterator<R, M>& _i2) :
		i1(_i1), i2(_i2)
	{
	}

	virtual ~IteratorNSSeqUnion()
	{
		delete &i1;
		delete &i2;
	}

	void first()
	{
		i1.first();
		i2.first();
	}

	void next()
	{
		if (!i1.isDone())
			i1.next();
		else
			i2.next();
	}

	bool isDone()
	{
		return i1.isDone() && i2.isDone();
	}

	Move<R, M>& current()
	{
		if (!i1.isDone())
			return *new MOVE(0, i1.current());
		else if (!i2.isDone())
			return *new MOVE(1, i2.current());
		else
			throw NSSeqUnionAdapterOutOfBound();
	}
};



#endif /*OPTFRAME_ITERATORNSSEQUNION_HPP_*/
