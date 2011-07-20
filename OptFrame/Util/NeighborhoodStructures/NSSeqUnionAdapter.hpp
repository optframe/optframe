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
#include "../../NSSeq.hpp"

#include "./Moves/MoveNSSeqUnion.hpp"
#include "./NSIterators/IteratorNSSeqUnion.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveNSSeqUnion<R, M> >
class NSSeqUnionAdapter: public NSSeq<R, M>
{
private:
	vector<NSSeq<R, M>*> ns;

public:

	NSSeqUnionAdapter(NSSeq<R, M>& _n1, NSSeq<R, M>& _n2)
	{
	   ns.push_back(&_n1);
	   ns.push_back(&_n2);
	}

	void add_ns(NSSeq<R, M>& _ns)
	{
	   ns.push_back(&_ns);
	}

	virtual ~NSSeqUnionAdapter()
	{
	}

	Move<R, M>& move(const R& r)
	{
      int x = rand() % ns.size();

      return *new MOVE(x, ns[x]->move(r));
   }

	virtual NSIterator<R, M>& getIterator(const R& r)
	{
      vector<NSIterator<R, M>*> it;
      for(unsigned int i = 0; i < ns.size(); i++)
         it.push_back(&ns[i]->getIterator(r));

      return *new IteratorNSSeqUnion<R, M, MOVE> (it);
   }

	virtual void print()
	{
		cout << "NSSeqUnionAdapter {" << endl;
		for(unsigned int i = 0; i < ns.size(); i++)
         ns[i]->print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQUNIONADAPTER_HPP_*/
