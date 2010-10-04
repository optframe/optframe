#ifndef OPTFRAME_FI_HPP_
#define OPTFRAME_FI_HPP_

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class FirstImprovement: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& eval;
	NSSeq<R, M>& nsSeq;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	FirstImprovement(Evaluator<R, M>& _eval, NSSeq<R, M>& _nsSeq) :
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
		NSIterator<R, M>& it = nsSeq.getIterator(s.getR());

		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		do
		{
			Move<R, M>* move = &it.current();

			if (move->canBeApplied(e, s) && eval.betterThan(eval.moveCost(e, *move, s), 0))
			{
				move->apply(e, s);
				delete move;

				delete &it;

				eval.evaluate(e, s); // updates 'e'
				return;
			}

			delete move;

			it.next();
		}
		while (!it.isDone());

		delete &it;
	}
};

#endif /*OPTFRAME_FI_HPP_*/
