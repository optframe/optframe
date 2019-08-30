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

#ifndef OPTFRAME_FI_SLOW_HPP_
#define OPTFRAME_FI_SLOW_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class FirstImprovementSlow: public LocalSearch<R, ADS>
{
private:
	Evaluator<R, ADS>& eval;
	NSSeq<R, ADS>& nsSeq;

public:

	FirstImprovementSlow(Evaluator<R, ADS>& _eval, NSSeq<R, ADS>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~FirstImprovementSlow()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation& e = eval.evaluate(s);
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f)
	{
		NSIterator<R, ADS>& it = nsSeq.getIterator(s);
		string bestMoveId = "";
		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		do
		{
			Move<R, ADS>* move = &it.current();

			// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
			/*
			if(e.getLocalOptimumStatus(move->id()))
			{
				delete &it;
				delete move;
				return;
			}
			*/

			bestMoveId = move->id();

			if (move->canBeApplied(s))
			{
				MoveCost* eCost = &eval.moveCost(e, *move, s); // estimated cost

				if(eval.isImprovement(*eCost))
				{
					if(eCost->isEstimated())
					{
						//double cost = eval.moveCost(e, *move, s); // real cost
						// TODO: find a real cost value...
					}

					if(eval.isImprovement(*eCost))
					{
						delete eCost;

						Component::safe_delete(move->apply(e, s));
						delete move;

						delete &it;

						eval.evaluate(e, s); // updates 'e'

						// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
						//e.setLocalOptimumStatus(bestMoveId, false); //set NS 'id' out of Local Optimum

						return;
					}

				}

				delete eCost;
			}

			delete move;

			it.next();
		}
		while (!it.isDone());

		// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
		//if(bestMoveId != "")
		//	e.setLocalOptimumStatus(bestMoveId, true); //set NS 'id' on Local Optimum

		delete &it;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS>::idComponent() << ":FISlow";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "FISlow: " << nsSeq.toString();
		return ss.str();
	}
};


}

#endif /*OPTFRAME_FI_SLOW_HPP_*/
