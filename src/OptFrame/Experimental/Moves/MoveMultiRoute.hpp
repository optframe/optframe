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

#ifndef OPTFRAME_MOVEMULTIROUTE_HPP_
#define OPTFRAME_MOVEMULTIROUTE_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class MoveMultiRoute: public Move<vector<vector<T> > , DS >
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

protected:
	int k; // route number
	Move<Route, DS >& m;

public:

	MoveMultiRoute(int _k, Move<Route, DS >& _m) :
		k(_k), m(_m)
	{
	}

	int get_k()
	{
		return k;
	}

	Move<Route, DS >& get_m()
	{
		return m;
	}

	virtual ~MoveMultiRoute()
	{
		delete &m;
	}

	bool canBeApplied(const MultiRoute& rep)
	{
		return m.canBeApplied(rep[k]);
	}

	Move<MultiRoute, DS >& apply(MultiRoute& rep)
	{
		return *new MoveMultiRoute<T, DS > (k, m.apply(rep[k]));
	}

	Move<MultiRoute, DS >& apply(DS& mem, MultiRoute& rep)
	{
		return *new MoveMultiRoute<T, DS > (k, m.apply(mem, rep[k]));
	}

	virtual bool operator==(const Move<MultiRoute, DS >& _m) const
	{
		const MoveMultiRoute<T, DS >& m1 = (const MoveMultiRoute<T, DS >&) _m;
		if (k == m1.k)
			return m == m1.m;
		else
			return false;
	}

	void print() const
	{
		cout << "MoveMultiRoute: k=" << k << "; move = ";
		m.print();
	}
};


#endif /*OPTFRAME_MOVEMULTIROUTE_HPP_*/
