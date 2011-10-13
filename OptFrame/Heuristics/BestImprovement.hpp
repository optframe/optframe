// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_BI_HPP_
#define OPTFRAME_BI_HPP_

#include "../HTrajectory.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class BestImprovement: public HTrajectory<R, ADS, M>
{
private:
	Evaluator<R, ADS, M>& eval;
	NSSeq<R, ADS, M>& nsSeq;

public:

	using HTrajectory<R, ADS, M>::exec; // prevents name hiding

	BestImprovement(Evaluator<R, ADS, M>& _eval, NSSeq<R, ADS, M>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = eval.evaluate(s.getR());

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		NSIterator<R, ADS, M>& it = nsSeq.getIterator(e.getEM(), s.getR());

		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		Move<R, ADS, M>* bestMove = &it.current();
		while (!bestMove->canBeApplied(e, s))
		{
			delete bestMove;
			it.next();
			if (!it.isDone())
			{
				bestMove = &it.current();
			}
			else
			{
				delete &it;
				return;
			}
		}

		double bestCost = eval.moveCost(e, *bestMove, s);
		it.next();
		while (!it.isDone())
		{
			Move<R, ADS, M>* move = &it.current();
			if (move->canBeApplied(e, s))
			{
				double cost = eval.moveCost(e, *move, s);

				if (eval.betterThan(cost, bestCost))
				{
					delete bestMove;
					bestMove = move;
					bestCost = cost;
				}
				else
					delete move;
			}
			else
				delete move;

			it.next();
		}
		bestMove->apply(e, s);
		delete bestMove;
		delete &it;

		eval.evaluate(e, s); // updates 'e'
	}

	virtual string id() const
   {
      return "OptFrame:BI:bi";
   }

};

#endif /*OPTFRAME_BI_HPP_*/
