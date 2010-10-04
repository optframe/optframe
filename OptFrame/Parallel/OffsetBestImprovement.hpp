#ifndef OffsetBestImprovement_HPP_
#define OffsetBestImprovement_HPP_

// TODO implement this file

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class OffsetBestImprovement: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& eval;
	NSSeq<R, M>& nsSeq;

	int init,step;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	OffsetBestImprovement(Evaluator<R, M>& _eval, NSSeq<R, M>& _nsSeq, int _init, int _step) :
		eval(_eval), nsSeq(_nsSeq), init(_init), step(_step)
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
		NSIterator<R, M>& it = nsSeq.getIterator(s.getR());

		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}


		// for statement added
		for (int i = 0 ; i <= init ; i++)
		{
			it.next();

			if (it.isDone())
			{
				delete &it;
				return;
			}
		}


		Move<R, M>* bestMove = &it.current();
		while (!bestMove->canBeApplied(s))
		{
			delete bestMove;
			//it.next();

			// for statement added
			for (int i = 0 ; i < step && !it.isDone() ; i++) it.next();

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

		//it.next();
		// for statement added
		for (int i = 0 ; i < step && !it.isDone() ; i++) it.next();

		while (!it.isDone())
		{
			Move<R, M>* move = &it.current();
			if (move->canBeApplied(s))
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

			//it.next();
			// for statement added
			for (int i = 0 ; i < step && !it.isDone() ; i++) it.next();
		}
		bestMove->apply(e, s);
		delete bestMove;
		delete &it;

		eval.evaluate(e, s); // updates 'e'
	}

};


#endif /*OffsetBestImprovement_HPP_*/
