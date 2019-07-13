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

#ifndef OPTFRAME_MOVEVVShiftkIntra_HPP_
#define OPTFRAME_MOVEVVShiftkIntra_HPP_

// Framework includes
#include "../../Move.hpp"

#include "../NSVector.hpp"

using namespace std;

//============================================================================
//                           VVShiftkIntra Move
//============================================================================

template<class T, class DS >
class MoveVVShiftkIntra : public Move<vector<vector<T> >, DS >
{
public:
	int k,v,p1,p2;

	MoveVVShiftkIntra(int k,int v,int p1,int p2)
	{
		this->k = k;
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
		pair<int,pair<int,int> > m;
		m.first = k;
		m.second.first = p1;
		m.second.second = p2;
		NSVector<T>::shiftk_apply(rep[v],m);

		return * new MoveVVShiftkIntra<T,DS >(k,v,p2,p1);
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
		cout << "Move VRP ShiftkIntra("<< k << " " << v << " " << p1 << " " << p2 <<")"<<endl;
	}

	virtual bool operator==(const Move<vector<vector<T> >,DS >& m) const
	{
		return false; //TODO
	}
};

#endif /*OPTFRAME_MOVEVVShiftkIntra_HPP_*/
