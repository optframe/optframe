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

#ifndef OPTFRAME_NSSEQ_PAIRSECOND_ADAPTER_HPP_
#define OPTFRAME_NSSEQ_PAIRSECOND_ADAPTER_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "Moves/MovePairSecond.hpp"
#include "NSIterators/IteratorNSSeqPairSecond.hpp"

using namespace std;

template<class T1, class T2, class DS = OPTFRAME_DEFAULT_EMEMORY, class MOVE = MovePairSecond<T1, T2, DS > >
class NSSeqPairSecondAdapter: public NSSeq<pair<T1, T2> , DS >
{
protected:
	NSSeq<T2, DS >& ns;

public:

	NSSeqPairSecondAdapter(NSSeq<T2, DS >& _ns) :
		ns(_ns)
	{
	}

	virtual ~NSSeqPairSecondAdapter()
	{
	}

	Move<pair<T1, T2> , DS >& move(const pair<T1, T2>& r)
	{
		return *new MOVE(ns.move(r.second));
	}

	virtual NSIterator<pair<T1, T2> , DS >& getIterator(const pair<T1, T2>& r)
	{
		return *new IteratorNSSeqPairSecond<T1, T2, DS, MOVE> (ns.getIterator(r.second));
	}

	virtual void print()
	{
		cout << "NSSeqPairSecondAdapter {" << endl;
		ns.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQ_PAIRSECOND_ADAPTER_HPP_*/
