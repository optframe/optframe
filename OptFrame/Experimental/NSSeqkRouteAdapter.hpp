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

#ifndef OPTFRAME_NSSEQkROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQkROUTEADAPTER_HPP_

// Framework includes
#include "../NSSeq.hpp"

#include "./Moves/MovekRoute.hpp"
#include "./NSIterators/IteratorNSSeqkRoute.hpp"

using namespace std;

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovekRoute<T, M> >
class NSSeqkRouteAdapter: public NSSeq<vector<vector<T> > , M>
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	int k;
	NSSeq<Route, M>& ns;

public:

	NSSeqkRouteAdapter(int _k, NSSeq<Route, M>& _ns) :
		k(_k), ns(_ns)
	{
	}

	virtual ~NSSeqkRouteAdapter()
	{
	}

	Move<MultiRoute, M>& move(const MultiRoute& r)
	{
		return *new MOVE(k, ns.move(r[k]));
	}

	virtual NSIterator<MultiRoute, M>& getIterator(const MultiRoute& r)
	{
		NSIterator<Route, M>& iterator = ns.getIterator(r[k]);
		return *new IteratorNSSeqkRoute<T, M, MOVE> (k, iterator);
	}

	virtual void print()
	{
		cout << "NSSeqkRouteAdapter {" << endl;
		ns.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQkROUTEADAPTER_HPP_*/
