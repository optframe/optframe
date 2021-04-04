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

#ifndef OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_
#define OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_

// Framework includes
#include "../../../Move.hpp"
#include "../../../NSIterator.hpp"

#include "../Moves/MoveTSP2Opt.hpp"

using namespace std;

// Working structure: vector<T>

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE = MoveTSPSwap<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>>
template<class T, class ADS, XBaseSolution<vector<T>,ADS> S, class MOVE = MoveTSP2Opt<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSIteratorTSP2Opt: public NSIterator<XES, XEv>
{
	typedef vector<T> Route;

protected:
	uptr<Move<XES, XEv>> m;

	int p1, p2; // position 1 and position 2, respectively
	const Route& r;

	std::shared_ptr<P> p; // has to be the last

public:

	NSIteratorTSP2Opt(const S& _s, std::shared_ptr<P> _p = nullptr) :
			r(_s.getR()), p(_p)
	{
		p1 = 0; // avoid non-initialization warning
		p2 = 2; // avoid non-initialization warning

		m = nullptr;
	}

	virtual ~NSIteratorTSP2Opt()
	{
	}

	virtual void first() override
	{
		if (r.size() >= 2)
		{
			p1 = 0;
			p2 = 2;
			m = uptr<MOVE>(new MOVE(p1, p2, p));
		}
		else
			m = nullptr;
	}

	virtual void next() override
	{
		if (!((p1 == ((int) r.size()) - 2) && (p2 == ((int) r.size()))))
		{
			if (p2 != ((int) r.size()))
			{
				p2++;
			}
			else
			{
				p1++;
				p2 = p1 + 2;
			}

      
         MOVE* pm = new MOVE(p1, p2, p);
         m = uptr<Move<XES>>(pm);
		}
		else
			m = nullptr;
	}

	virtual bool isDone() override
	{
		return m == nullptr;
	}

	virtual uptr<Move<XES, XEv>> current() override
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqRouteShift. Aborting." << endl;
			exit(1);
		}

      uptr<Move<XES>> m2 = std::move(m);
      m = nullptr;

		return m2;
	}
};

#endif /*OPTFRAME_ITERATORNSSEQTSP2OPT_HPP_*/
