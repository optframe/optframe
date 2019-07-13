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

#ifndef OPTFRAME_MOVEVVSWAPINTRA_HPP_
#define OPTFRAME_MOVEVVSWAPINTRA_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class MoveVVSwapIntra: public Move<vector<vector<T> > , DS >
{
	typedef vector<vector<T> > Routes;

private:
	int i, j, k;

public:

	MoveVVSwapIntra(int i, int j, int k)
	{
		this->i = i;
		this->j = j;
		this->k = k;
	}

	virtual ~MoveVVSwapIntra()
	{
	}

	bool canBeApplied(const Routes& rep)
	{
		return (j != k);
	}

	Move<Routes, DS >& apply(Routes& rep)
	{
		T aux = rep[i][j];
		rep[i][j] = rep[i][k];
		rep[i][k] = aux;

		return *new MoveVVSwapIntra(i, k, j);
	}

	virtual bool operator==(const Move<Routes, DS >& _m) const
	{
		const MoveVVSwapIntra& m = (const MoveVVSwapIntra&) _m;
		return (m.i == i) && ((m.j == j && m.k == k) || (m.j == k && m.k == j));
	}

	void print() const
	{
		cout << "MoveVVSwapIntra( " << i << " , ( " << j << " , " << k << " ) )" << endl;
	}
};

#endif /*OPTFRAME_MOVEVVSWAPINTRA_HPP_*/
