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

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveNSSeqUnion: public Move<XES, XEv>
{
protected:
	int id;
	uptr<Move<XES, XEv>> m {nullptr};

public:

/*
	MoveNSSeqUnion(int _id, uptr<Move<XES, XEv>>&& _m) :
			id(_id), m{std::move(_m)}
	{
	}
*/


	MoveNSSeqUnion(int _id, uptr<Move<XES, XEv>> _m) :
			//id(_id), m(std::move(_m))
         id(_id), m(_m)
	{
	}


	int get_id()
	{
		return id;
	}

	uptr<Move<XES, XEv>>& get_m()
	{
		return m;
	}

	virtual ~MoveNSSeqUnion()
	{
		//delete &m;
	}

	bool canBeApplied(const XES& s) override
	{
		return m->canBeApplied(s);
	}

	uptr<Move<XES, XEv>> apply(XES& se) override
	{
		return uptr<Move<XES, XEv>>( new MoveNSSeqUnion<S, XEv, XES>(id, m->apply(se)) );
	}

	uptr<Move<XES, XEv>> applyUpdate(Evaluation<>& e, S& s)
	{
		return uptr<Move<XES, XEv>>( new MoveNSSeqUnion<S, XEv, XES>(id, m->apply(e, s)) );
	}

	virtual bool operator==(const Move<XES, XEv>& _m) const
	{
		const MoveNSSeqUnion<S, XEv, XES>& m1 = (const MoveNSSeqUnion<S, XEv, XES>&) _m;
		if (id == m1.id)
			return *m == *(m1.m);
		else
			return false;
	}

	void print() const override
	{
		cout << "MoveNSSeqUnion: id=" << id << "; move = ";
		m->print();
	}
};

#endif /*OPTFRAME_MOVENSSEQUNION_HPP_*/
