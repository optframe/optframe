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

#ifndef OPTFRAME_ITERATORNSSEQTSPOR1OPT_HPP_
#define OPTFRAME_ITERATORNSSEQTSPOR1OPT_HPP_

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MoveTSPOr1Opt.hpp"

using namespace std;

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class IteratorNSSeqTSPOr1Opt: public NSIterator<vector<T> , DS >
{

private:

	typedef vector<T> Route;

	MoveTSPOr1Opt<T, DS >* m;
	vector<MoveTSPOr1Opt<T,DS >*> moves;
	int index; //index of moves
	const Route& rep;

public:

	IteratorNSSeqTSPOr1Opt(const Route& _r) :
		rep(_r)
	{
		m = nullptr;
		index = 0;
	}

	virtual ~IteratorNSSeqTSPOr1Opt()
	{
	}

	void first()
	{

		for (int c = 0; c < rep.size(); c++)
		{
			for (int pos = 0; pos <= rep.size(); pos++)
			{
				if ((c != pos) && (c + 1 != pos))
				{
					moves.push_back(new MoveTSPOr1Opt<T, DS >(c, pos));
				}
			}
		}

		if (moves.size() > 0)
		{
			m = moves[index];
		}
		else
			m = nullptr;
	}

	void next()
	{
		index++;
		if (index < moves.size())
		{
			m = moves[index];
		}
		else
			m = nullptr;
	}

	bool isDone()
	{
		return m == nullptr;
	}

	MoveTSPOr1Opt<T, DS >& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "TSPOr1Opt. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};
#endif /*OPTFRAME_ITERATORNSSEQTSPOR1OPT_HPP_*/
