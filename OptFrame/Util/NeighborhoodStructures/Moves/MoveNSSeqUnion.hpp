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

#ifndef OPTFRAME_MOVENSSEQUNION_HPP_
#define OPTFRAME_MOVENSSEQUNION_HPP_

// Framework includes
#include "../../../Move.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_EMEMORY>
class MoveNSSeqUnion: public Move<R, M>
{
protected:
	int id;
	Move<R, M>& m;

public:

	MoveNSSeqUnion(int _id, Move<R, M>& _m) :
		id(_id), m(_m)
	{
	}

	int get_id()
	{
		return id;
	}

	Move<R, M>& get_m()
	{
		return m;
	}

	virtual ~MoveNSSeqUnion()
	{
		delete &m;
	}

	bool canBeApplied(const R& r)
	{
		return m.canBeApplied(r);
	}

	Move<R, M>& apply(R& r)
	{
		return *new MoveNSSeqUnion<R, M> (id, m.apply(r));
	}

	Move<R, M>& apply(M& mem, R& rep)
	{
		return *new MoveNSSeqUnion<R, M> (id, m.apply(mem, rep));
	}

	virtual bool operator==(const Move<R, M>& _m) const
	{
		const MoveNSSeqUnion<R, M>& m1 = (const MoveNSSeqUnion<R, M>&) _m;
		if (id == m1.id)
			return m == m1.m;
		else
			return false;
	}

	void print() const
	{
		cout << "MoveNSSeqUnion: id=" << id << "; move = ";
		m.print();
	}
};

#endif /*OPTFRAME_MOVENSSEQUNION_HPP_*/
