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

#ifndef OPTFRAME_MOVENSSEQUNION_HPP_
#define OPTFRAME_MOVENSSEQUNION_HPP_

// Framework includes
#include "../../../Move.hpp"

using namespace std;

template<XSolution S, XEvaluation XEv = Evaluation<>>
class MoveNSSeqUnion: public Move<S, XEv>
{
protected:
	int id;
	Move<S, XEv>& m;

public:

	MoveNSSeqUnion(int _id, Move<S, XEv>& _m) :
			id(_id), m(_m)
	{
	}

	int get_id()
	{
		return id;
	}

	Move<S, XEv>& get_m()
	{
		return m;
	}

	virtual ~MoveNSSeqUnion()
	{
		delete &m;
	}

	bool canBeApplied(const S& s) override
	{
		return m.canBeApplied(s);
	}

	Move<S, XEv>& apply(S& s) override
	{
		return *new MoveNSSeqUnion<S, XEv>(id, m.apply(s));
	}

	Move<S, XEv>& apply(Evaluation<>& e, S& s)
	{
		return *new MoveNSSeqUnion<S, XEv>(id, m.apply(e, s));
	}

	virtual bool operator==(const Move<S, XEv>& _m) const
			{
		const MoveNSSeqUnion<S, XEv>& m1 = (const MoveNSSeqUnion<S, XEv>&) _m;
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
