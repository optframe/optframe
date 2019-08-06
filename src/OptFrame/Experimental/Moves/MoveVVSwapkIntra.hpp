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

#ifndef OPTFRAME_MOVEVVSwapkIntra_HPP_
#define OPTFRAME_MOVEVVSwapkIntra_HPP_

// Framework includes
#include "../../Move.hpp"

#include "../NSVector.hpp"

using namespace std;

//============================================================================
//                           VVSwapkIntra Move
//============================================================================

template<class T, class DS >
class MoveVVSwapkIntra : public Move<vector<vector<T> >, DS >
{
public:
	int k1,k2,v,p1,p2;

	MoveVVSwapkIntra(int k1,int k2,int v,int p1,int p2)
	{
		this->k1 = k1;
		this->k2 = k2;
		this->v = v;
		this->p1 = p1;
		this->p2 = p2;
	}

	virtual bool canBeApplied(const vector<vector<T> >&)
	{
		return true;
	}

	virtual Move<vector<vector<T> >, DS >& apply(vector<vector<T> >& rep)
	{
		pair<pair<int,int>,pair<int,int> > m;
		m.first.first = k1;
		m.first.second = k2;
		m.second.first = p1;
		m.second.second = p2;
		NSVector<T>::swapk_apply(rep[v],m); //TODO

		return * new MoveVVSwapkIntra<T,DS >(k1,k2,v,p2,p1);
	}

	virtual Move<vector<vector<T> >, DS >& apply(DS& m, vector<vector<T> > & r)
	{
		if (!m.empty())
		{
			m[v].first = -1;
			m[v].second.first = p1;
			m[v].second.second = r[v].size()-1;
		} else
		{
			//e->setMemory(new MemVRP(r.size(),make_pair(-1,make_pair(0,r.size()-1))));
			m = MemVRPTW(r.size(),make_pair(-1,make_pair(0,r.size()-1)));
		}

		return apply(r);
	}

	virtual void print() const
	{
		cout << "Move VRP SwapkIntra("<< k1 << " " << k2 << " " << v << " " << p1 << " " << p2 <<")"<<endl;
	}

	virtual bool operator==(const Move<vector<vector<T> >,DS >& m) const
	{
		return false; //TODO
	}
};

#endif /*OPTFRAME_MOVEVVSwapkIntra_HPP_*/
