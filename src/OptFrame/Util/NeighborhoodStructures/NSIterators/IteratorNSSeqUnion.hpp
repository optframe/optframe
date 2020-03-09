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

#ifndef OPTFRAME_ITERATORNSSEQUNION_HPP_
#define OPTFRAME_ITERATORNSSEQUNION_HPP_

// Framework includes

#include "../../../Move.hpp"
#include "../../../NSIterator.hpp"
#include "../Moves/MoveNSSeqUnion.hpp"

using namespace std;

class NSSeqUnionAdapterOutOfBound
{
public:
	NSSeqUnionAdapterOutOfBound()
	{
	}
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE = MoveTSPSwap<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>>
//template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveNSSeqUnion<R, ADS>,  >
template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, class MOVE = MoveNSSeqUnion<S, XEv, XES>>
class IteratorNSSeqUnion : public NSIterator<XES, XEv>
{
private:
	vector<uptr<NSIterator<XES>>> it;
	int k;

public:

/*
   // ERROR! CANNOT PASS uptr CORRECTLY!
	IteratorNSSeqUnion(vector<uptr<NSIterator<XES>>>&& _it) :
			it(_it)
	{
		k = 0;
	}
*/
   // these pointers are passed together with ownership!! never delete them!
	IteratorNSSeqUnion(vector<NSIterator<XES>*> _it)
	{
		k = 0;
      for(unsigned i=0; i<_it.size(); i++)
         it.push_back(uptr<NSIterator<XES>>(_it[i]));
	}

	virtual ~IteratorNSSeqUnion()
	{
		//for (unsigned int i = 0; i < it.size(); i++)
		//	delete it[i];
	}

	void first() override
	{
		for (unsigned int i = 0; i < it.size(); i++)
			it[i]->first();
		k = 0;
	}

	void next() override
	{
		if (!it[k]->isDone())
			it[k]->next();
		else
		{
			k++;
			it[k]->next();
		}
	}

	bool isDone() override
	{
		for (unsigned int i = 0; i < it.size(); i++)
			if (!it[i]->isDone())
				return false;

		return true;
	}

	//Move<R, ADS>& current() override
   uptr<Move<XES>> current() override
	{
		if (!it[k]->isDone())
			return uptr<Move<XES>>( new MOVE(k, it[k]->current().release()) );
		else
			throw NSSeqUnionAdapterOutOfBound();
	}
};

#endif /*OPTFRAME_ITERATORNSSEQUNION_HPP_*/
