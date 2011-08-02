// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_MOVE_TSP_OROPTK_HPP_
#define OPTFRAME_MOVE_TSP_OROPTK_HPP_

// Framework includes
#include "../../../Move.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveTSPOrOptk: public Move<vector<T> , M>
{
	typedef vector<T> Route;

private:
	int i; // origin
	int j; // destination
	int k; // number of elements

public:

	MoveTSPOrOptk(int _i, int _j, int _k) :
		i(_i), j(_j), k(_k)
	{
	}

	virtual ~MoveTSPOrOptk()
	{
	}

	bool canBeApplied(const Route& rep)
	{
		//return (i != j) && (i + k <= rep.size());
		return (i != j);
	}

	Move<Route, M>& apply(Route& rep)
	{
		vector<T> v_aux;
		v_aux.insert(v_aux.begin(), rep.begin() + i, rep.begin() + i + k);
		rep.erase(rep.begin() + i, rep.begin() + i + k);
		rep.insert(rep.begin() + j, v_aux.begin(), v_aux.end());

		return *new MoveTSPOrOptk(j, i, k);
	}

	virtual bool operator==(const Move<Route, M>& _m) const
	{
		const MoveTSPOrOptk& m1 = (const MoveTSPOrOptk&) _m;
		return (m1.i == i) && (m1.j == j) && (m1.k == k);
	}

	void print() const
	{
		cout << "MoveTSPOrOpt{K=" << k << "}";
		cout << "(" << i << ";" << j << ")" << endl;
	}
};

#endif /*OPTFRAME_MOVE_TSP_OROPTK_HPP_*/
