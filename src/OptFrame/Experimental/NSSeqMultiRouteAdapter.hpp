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

#ifndef OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_

// Framework includes

#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "Moves/MoveMultiRoute.hpp"
#include "NSIterators/IteratorNSSeqMultiRoute.hpp"

using namespace std;

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MoveMultiRoute<T, DS > >
class NSSeqMultiRouteAdapter: public NSSeq<vector<vector<T> > , DS >
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	NSSeq<Route, DS >& ns;

public:

	NSSeqMultiRouteAdapter(NSSeq<Route, DS >& _ns) :
		ns(_ns)
	{
	}

	virtual ~NSSeqMultiRouteAdapter()
	{
	}

	Move<MultiRoute, DS >& move(const MultiRoute& r)
	{
		int x = rand() % r.size();
		return *new MOVE(x, ns.move(r[x]));
	}

	virtual NSIterator<MultiRoute, DS >& getIterator(const MultiRoute& r)
	{
		vector<NSIterator<Route, DS >*>& iterators = *new vector<NSIterator<Route, DS >*> ;
		for (int i = 0; i < r.size(); i++)
			iterators.push_back(&ns.getIterator(r[i]));

		return *new IteratorNSSeqMultiRoute<T, DS, MOVE> (iterators);
	}

	virtual void print()
	{
		cout << "NSSeqMultiRouteAdapter {" << endl;
		ns.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_*/
