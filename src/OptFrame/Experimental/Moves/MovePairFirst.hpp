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

#ifndef OPTFRAME_MOVEPAIRFIRST_ADAPTER_HPP_
#define OPTFRAME_MOVEPAIRFIRST_ADAPTER_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template<class T1, class T2, class DS = OPTFRAME_DEFAULT_EMEMORY>
class MovePairFirst: public Move<pair<T1, T2> , DS >
{
protected:
	Move<T1, DS >& m;

public:

	MovePairFirst(Move<T1, DS >& _m) :
		m(_m)
	{
	}

	Move<T1, DS >& get_m()
	{
		return m;
	}

	virtual ~MovePairFirst()
	{
		delete &m;
	}

	bool canBeApplied(const pair<T1, T2>& rep)
	{
		return m.canBeApplied(rep.first);
	}

	Move<pair<T1, T2> , DS >& apply(pair<T1, T2>& rep)
	{
		return *new MovePairFirst<T1, T2, DS > (m.apply(rep.first));
	}

	virtual bool operator==(const Move<pair<T1, T2> , DS >& _m) const
	{
		const MovePairFirst<T1, T2, DS >& m1 = (const MovePairFirst<T1, T2, DS >&) _m;
		return m == m1.m;
	}

	void print() const
	{
		cout << "MovePairFirst: move = ";
		m.print();
	}
};

#endif /*OPTFRAME_MOVEPAIRFIRST_ADAPTER_HPP_*/
