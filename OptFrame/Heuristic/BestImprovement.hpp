#ifndef OPTFRAME_BI_HPP_
#define OPTFRAME_BI_HPP_

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class BestImprovement: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& eval;
	NSSeq<R, M>& nsSeq;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	BestImprovement(Evaluator<R, M>& _eval, NSSeq<R, M>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = eval.evaluate(s.getR());

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		NSIterator<R, M>& it = nsSeq.getIterator(e.getM(), s.getR());

		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		Move<R, M>* bestMove = &it.current();
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
			Move<R, M>* move = &it.current();
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

};

#endif /*OPTFRAME_BI_HPP_*/
