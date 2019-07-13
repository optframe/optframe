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

#ifndef OPTFRAME_NSSEQkROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQkROUTEADAPTER_HPP_

// Framework includes
#include "../NSSeq.hpp"

#include "./Moves/MovekRoute.hpp"
#include "./NSIterators/IteratorNSSeqkRoute.hpp"

using namespace std;

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MovekRoute<T, DS > >
class NSSeqkRouteAdapter: public NSSeq<vector<vector<T> > , DS >
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	int k;
	NSSeq<Route, DS >& ns;

public:

	NSSeqkRouteAdapter(int _k, NSSeq<Route, DS >& _ns) :
		k(_k), ns(_ns)
	{
	}

	virtual ~NSSeqkRouteAdapter()
	{
	}

	Move<MultiRoute, DS >& move(const MultiRoute& r)
	{
		return *new MOVE(k, ns.move(r[k]));
	}

	virtual NSIterator<MultiRoute, DS >& getIterator(const MultiRoute& r)
	{
		NSIterator<Route, DS >& iterator = ns.getIterator(r[k]);
		return *new IteratorNSSeqkRoute<T, DS, MOVE> (k, iterator);
	}

	virtual void print()
	{
		cout << "NSSeqkRouteAdapter {" << endl;
		ns.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQkROUTEADAPTER_HPP_*/
