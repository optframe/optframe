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

#ifndef OPTFRAME_MOVE_TSP_OROPTK_HPP_
#define OPTFRAME_MOVE_TSP_OROPTK_HPP_

// Framework includes
#include "../../../Move.hpp"

using namespace std;
using namespace optframe;

// Working structure: vector<vector<T> >

template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
class MoveTSPOrOptk: public Move<vector<T>, ADS>
{
	typedef vector<T> Route;

protected:
	int i; // origin
	int j; // destination
	int k; // number of elements

	OPTFRAME_DEFAULT_PROBLEM* problem;

public:

	using Move<vector<T>, ADS>::apply;
	using Move<vector<T>, ADS>::canBeApplied;

	MoveTSPOrOptk(int _i, int _j, int _k, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
			i(_i), j(_j), k(_k), problem(_problem)
	{
	}

	virtual ~MoveTSPOrOptk()
	{
	}

	int get_i() const
	{
		return i;
	}

	int get_j() const
	{
		return j;
	}

	int get_k() const
	{
		return k;
	}

	virtual bool canBeApplied(const Route& rep, const ADS&)
	{
		//return (i != j) && (i + k <= rep.size());
		return abs(i - j) >= k;
	}

	virtual Move<Route, ADS>* apply(Route& rep, ADS&)
	{
		vector<T> v_aux;
		v_aux.insert(v_aux.begin(), rep.begin() + i, rep.begin() + i + k);
		rep.erase(rep.begin() + i, rep.begin() + i + k);
		rep.insert(rep.begin() + j, v_aux.begin(), v_aux.end());

		return new MoveTSPOrOptk(j, i, k);
	}

	virtual bool operator==(const Move<Route, ADS>& _m) const
			{
		const MoveTSPOrOptk& m1 = (const MoveTSPOrOptk&) _m;
		return (m1.i == i) && (m1.j == j) && (m1.k == k);
	}

	static string idComponent()
	{
		string idComp = Move<vector<T>, ADS>::idComponent();
		idComp.append("MoveTSPOrOptk");
		return idComp;
	}

	virtual void print() const
	{
		cout << "MoveTSPOrOpt{K=" << k << "}";
		cout << "(" << i << ";" << j << ")" << endl;
	}
};

#endif /*OPTFRAME_MOVE_TSP_OROPTK_HPP_*/
