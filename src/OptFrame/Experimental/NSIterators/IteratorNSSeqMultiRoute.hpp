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

#ifndef OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_
#define OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSIterator.hpp"

using namespace std;

class NSSeqMultiRouteIteratorOutOfBound
{
public:
	NSSeqMultiRouteIteratorOutOfBound()
	{
	}
};

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MoveMultiRoute<T, DS > >
class IteratorNSSeqMultiRoute: public NSIterator<vector<vector<T> > , DS >
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	vector<NSIterator<Route, DS >*>& iterators;
	int i;

public:

	IteratorNSSeqMultiRoute(vector<NSIterator<Route, DS >*>& it) :
		iterators(it)
	{
		i = 0;
	}

	virtual ~IteratorNSSeqMultiRoute()
	{
		for (int j = 0; j < iterators.size(); j++)
			delete iterators[j];
		delete &iterators;
	}

	void first()
	{
		for (int j = 0; j < iterators.size(); j++)
			iterators[j]->first();

		i = 0;
		while (i < iterators.size())
			if (!iterators[i]->isDone())
				break;
			else
				i++;
	}

	void next()
	{
		iterators[i]->next();
		while (i < iterators.size())
			if (!iterators[i]->isDone())
				break;
			else
				i++;
	}

	bool isDone()
	{
		for (int j = i; j < iterators.size(); j++)
			if (!iterators[j]->isDone())
				return false;
		return true;
	}

	Move<MultiRoute, DS >& current()
	{
		if ((i < iterators.size()) && (!iterators[i]->isDone()))
			return *new MOVE(i, iterators[i]->current());
		else
			throw NSSeqMultiRouteIteratorOutOfBound();
	}
};


#endif /*OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_*/
