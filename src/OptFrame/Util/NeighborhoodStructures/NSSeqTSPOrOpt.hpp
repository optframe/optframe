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

#ifndef OPTFRAME_NSSEQVECTOROROPT_HPP_
#define OPTFRAME_NSSEQVECTOROROPT_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSSeq.hpp"

#include "BaseSolutionTSP.hpp"

#include "NSSeqTSPOrOptk.hpp"
#include "NSSeqUnionAdapter.hpp"

using namespace std;

// definition: OrOpt is OrOpt{1..3}

namespace optframe
{

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE = MoveTSPSwap<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPSwap<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>>
template<class T, class ADS, XBaseSolution<vector<T>,ADS> S, class MOVE = MoveTSPSwap<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPSwap<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqTSPOrOpt: public NSSeq<XES, XEv, XSH>
{
	typedef vector<T> Route;

	NSSeqTSPOrOptk<T, ADS, S>* OrOpt1;
	NSSeqTSPOrOptk<T, ADS, S>* OrOpt2;
	NSSeqTSPOrOptk<T, ADS, S>* OrOpt3;

	NSSeqUnionAdapter<vector<T>, ADS>* OrOpt1_2_3;

public:

	NSSeqTSPOrOpt()
	{
		OrOpt1 = new NSSeqTSPOrOptk<T, ADS, S>(1);
		OrOpt2 = new NSSeqTSPOrOptk<T, ADS, S>(2);
		OrOpt3 = new NSSeqTSPOrOptk<T, ADS, S>(3);

		OrOpt1_2_3 = new NSSeqUnionAdapter<vector<T>, ADS>(*OrOpt1, *OrOpt2);
		OrOpt1_2_3->add_ns(*OrOpt3);
	}

	virtual ~NSSeqTSPOrOpt()
	{
		delete OrOpt1;
		delete OrOpt2;
		delete OrOpt3;

		delete OrOpt1_2_3;
	}

   // Maybe S& should be the Representation itself.... no getR() here.
   // It makes more sense to pass RepTSP + ESolutionTSP... than SolutionTSP + ESolutionTSP
	Move<S, XEv>* randomMove(const S& s) override
	{
      const Route& rep = s.getR();
		return OrOpt1_2_3->move(s);
	}

	virtual Move<S, XEv>* validMove(const S& s) override
	{
      const Route& r = s.getR();
		Move<S, XEv>* m = &move(s);
		if (m->canBeApplied(s))
			return m;
		else
		{
			delete m;
			return nullptr;
		}
	}

	virtual NSIterator<S, XEv>* getIterator(const S& s) override
	{
		return OrOpt1_2_3->getIterator(s);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NSSeq<XES, XEv, XSH>::idComponent() << ":NSSeqTSPOrOpt";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (NSSeq<XES, XEv, XSH>::compatible(s));
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqVectorOrOpt{1..3}";
		return ss.str();
	}
};

} // namespace optframe

#endif /*OPTFRAME_NSSEQVECTOROROPT_HPP_*/
