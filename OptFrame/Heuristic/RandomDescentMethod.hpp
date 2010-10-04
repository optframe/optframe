#ifndef OPTFRAME_RDM_HPP_
#define OPTFRAME_RDM_HPP_

#include "../Heuristic.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class RandomDescentMethod: public Heuristic<R, M>
{
private:
	NS<R, M>& ns;
	Evaluator<R, M>& evaluator;
	unsigned int iterMax;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	RandomDescentMethod(Evaluator<R, M>& _eval, NS<R, M>& _ns, unsigned int _iterMax) :
		evaluator(_eval), ns(_ns), iterMax(_iterMax)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = evaluator.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		long tini = time(NULL);

		unsigned int iter = 0;

		long tnow = time(NULL);

		while (iter < iterMax && ((tnow - tini) < timelimit))
		{
			Move<R, M>& move = ns.move(s);

			double cost = 0;

			if (move.canBeApplied(e, s))
			{
				cost = evaluator.moveCost(e, move, s);
			}
			else
			{
				iter++;
				delete &move;
				tnow = time(NULL);
				continue;
			}

			iter++;

			if (evaluator.betterThan(cost, 0))
			{
				move.apply(e, s);
				iter = 0;
			}

			delete &move;
			tnow = time(NULL);
		}
	}
};

#endif /*OPTFRAME_RDM_HPP_*/
