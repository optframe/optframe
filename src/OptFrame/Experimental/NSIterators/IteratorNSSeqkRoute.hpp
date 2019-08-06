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

#ifndef OPTFRAME_ITERATORNSSEQKROUTE_HPP_
#define OPTFRAME_ITERATORNSSEQKROUTE_HPP_

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MovekRoute.hpp"


using namespace std;

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MovekRoute<T, DS > >
class IteratorNSSeqkRoute: public NSIterator<vector<vector<T> > , DS >
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	int k;
	NSIterator<Route, DS >& iterator;

public:

	IteratorNSSeqkRoute(int _k, NSIterator<Route, DS >& it) :
		k(_k), iterator(it)
	{
	}

	virtual ~IteratorNSSeqkRoute()
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

	Move<MultiRoute, DS >& current()
	{
		return *new MOVE(k, iterator.current());
	}
};

#endif /*OPTFRAME_ITERATORNSSEQKROUTE_HPP_*/
