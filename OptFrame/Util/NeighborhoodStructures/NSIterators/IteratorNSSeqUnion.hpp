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

#ifndef OPTFRAME_ITERATORNSSEQUNION_HPP_
#define OPTFRAME_ITERATORNSSEQUNION_HPP_

// Framework includes

#include "../../../NSIterator.hpp"
#include "../Moves/MoveNSSeqUnion.hpp"

using namespace std;

class NSSeqUnionAdapterOutOfBound
{
public:
	NSSeqUnionAdapterOutOfBound()
	{
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveNSSeqUnion<R, ADS> >
class IteratorNSSeqUnion:
		public NSIterator<R, ADS>
{
private:
	vector<NSIterator<R, ADS>*> it;
	int k;

public:

	IteratorNSSeqUnion(vector<NSIterator<R, ADS>*> _it) :
			it(_it)
	{
		k = 0;
	}

	virtual ~IteratorNSSeqUnion()
	{
		for (unsigned int i = 0; i < it.size(); i++)
			delete it[i];
	}

	void first()
	{
		for (unsigned int i = 0; i < it.size(); i++)
			it[i]->first();
		k = 0;
	}

	void next()
	{
		if (!it[k]->isDone())
			it[k]->next();
		else
		{
			k++;
			it[k]->next();
		}
	}

	bool isDone()
	{
		for (unsigned int i = 0; i < it.size(); i++)
			if (!it[i]->isDone())
				return false;

		return true;
	}

	Move<R, ADS>& current()
	{
		if (!it[k]->isDone())
			return *new MOVE(k, it[k]->current());
		else
			throw NSSeqUnionAdapterOutOfBound();
	}
};

#endif /*OPTFRAME_ITERATORNSSEQUNION_HPP_*/
