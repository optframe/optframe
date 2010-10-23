#ifndef OffsetBestImprovementEnum_HPP_
#define OffsetBestImprovementEnum_HPP_

// TODO implement this file

#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class OffsetBestImprovementEnum: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& eval;
	NSEnum<R, M>& nsEnum;

	int init,step;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	OffsetBestImprovementEnum(Evaluator<R, M>& _eval, NSEnum<R, M>& _nsEnum, int _init, int _step) :
		eval(_eval), nsEnum(_nsEnum), init(_init), step(_step)
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
		Move<R, M>* bestMove = &nsEnum.move(init);
		double bestCost = eval.moveCost(e, *bestMove, s);

		for (int i = init+step ; i < nsEnum.size() ; i += step)
		{
			Move<R, M>* move = &nsEnum.move(i);
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
		}
		Move<R, M>* rev = &bestMove->apply(e, s);
		delete rev;
		delete bestMove;

		eval.evaluate(e, s); // updates 'e'
	}

};


#endif /*OffsetBestImprovementEnum_HPP_*/
