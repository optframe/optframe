// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef OPTFRAME_NSSEQUNIONADAPTER_HPP_
#define OPTFRAME_NSSEQUNIONADAPTER_HPP_

// Framework includes
//#include "../../../Move.hpp"
#include "../NSSeq.hpp"

#include "./Moves/MoveNSSeqUnion.hpp"
#include "./NSIterators/IteratorNSSeqUnion.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveNSSeqUnion<R, M> >
class NSSeqUnionAdapter: public NSSeq<R, M>
{
private:
	NSSeq<R, M>& n1;
	NSSeq<R, M>& n2;

public:

	NSSeqUnionAdapter(NSSeq<R, M>& _n1, NSSeq<R, M>& _n2) :
		n1(_n1), n2(_n2)
	{
	}

	virtual ~NSSeqUnionAdapter()
	{
	}

	Move<R, M>& move(const R& r)
	{
		int x = rand() % 2;

		if (x == 0)
			return *new MOVE(0, n1.move(r));
		else
			return *new MOVE(1, n2.move(r));
	}

	virtual NSIterator<R, M>& getIterator(const R& r)
	{
		return *new IteratorNSSeqUnion<R, M, MOVE> (n1.getIterator(r), n2.getIterator(r));
	}

	virtual void print()
	{
		cout << "NSSeqUnionAdapter {" << endl;
		n1.print();
		n2.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQUNIONADAPTER_HPP_*/
