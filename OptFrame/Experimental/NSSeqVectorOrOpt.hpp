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

#ifndef OPTFRAME_NSSEQVECTOROROPT_HPP_
#define OPTFRAME_NSSEQVECTOROROPT_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "NSSeqVectorOrOptk.hpp"
#include "NSSeqUnionAdapter.hpp"

using namespace std;

// definition: OrOpt is OrOpt{1..3}

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSSeqVectorOrOpt: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;
	NSSeq<Route, M>* ns;

	NSSeqVectorOrOptk<T, M>* OrOpt1;
	NSSeqVectorOrOptk<T, M>* OrOpt2;
	NSSeqVectorOrOptk<T, M>* OrOpt3;

	NSSeqUnionAdapter<vector<T>, M>* OrOpt2_3;
	NSSeqUnionAdapter<vector<T>, M>* OrOpt1_2_3;
public:

	NSSeqVectorOrOpt()
	{
		OrOpt1 = new NSSeqVectorOrOptk<T, M> (1);
		OrOpt2 = new NSSeqVectorOrOptk<T, M> (2);
		OrOpt3 = new NSSeqVectorOrOptk<T, M> (3);

		OrOpt2_3 = new NSSeqUnionAdapter<vector<T>, M> (*OrOpt2, *OrOpt3);
		OrOpt1_2_3 = new NSSeqUnionAdapter<vector<T>, M> (*OrOpt1, *OrOpt2_3);
	}

	virtual ~NSSeqVectorOrOpt()
	{
		delete OrOpt1;
		delete OrOpt2;
		delete OrOpt3;

		delete OrOpt2_3;
		delete OrOpt1_2_3;
	}

	Move<Route, M>& move(const Route& rep)
	{
		return OrOpt1_2_3->move(rep);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& rep)
	{
		return OrOpt1_2_3->getIterator(rep);
	}

	virtual void print()
	{
		cout << "NSSeqVectorOrOpt{1..3}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQVECTOROROPT_HPP_*/
