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

#ifndef OPTFRAME_MOVEKROUTE_HPP_
#define OPTFRAME_MOVEKROUTE_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template<class T, class DS >
class MovekRoute: public Move<vector<vector<T> > , DS >
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

protected:
	int k; // route number
	Move<Route, DS >& m;

public:

	MovekRoute(int _k, Move<Route, DS >& _m) :
		k(_k), m(_m)
	{
	}

	virtual ~MovekRoute()
	{
		delete &m;
	}

	bool canBeApplied(const MultiRoute& rep)
	{
		return m.canBeApplied(rep[k]);
	}

	Move<MultiRoute, DS >& apply(MultiRoute& rep)
	{
		return *new MovekRoute<T, DS > (k, m.apply(rep[k]));
	}

	Move<MultiRoute, DS >& apply(DS& mem, MultiRoute& rep)
	{
		return *new MovekRoute<T, DS > (k, m.apply(mem, rep[k]));
	}

	virtual bool operator==(const Move<MultiRoute, DS >& _m) const
	{
		const MovekRoute<T, DS >& m1 = (const MovekRoute<T, DS >&) _m;
		if (k == m1.k)
			return m == m1.m;
		else
			return false;
	}

	void print() const
	{
		cout << "MovekRoute: k=" << k << "; move = ";
		m.print();
	}
};


#endif /*OPTFRAME_MOVEKROUTE_HPP_*/
