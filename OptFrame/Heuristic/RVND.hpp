#ifndef RVND_HPP_
#define RVND_HPP_

#include <algorithm>

#include "../Heuristic.hpp"
#include "../Evaluator.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class RVND: public Heuristic<R, M>
{
public:
	using Heuristic<R, M>::exec; // prevents name hiding

	RVND(Evaluator<R, M>& _ev, vector<Heuristic<R, M>*> _neighbors) :
		ev(_ev), neighbors(_neighbors)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = ev.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit,
			double target_f)
	{

		long tini = time(NULL);

		std::random_shuffle(neighbors.begin(), neighbors.end()); // shuffle elements
		int r = neighbors.size();

		int k = 1;

		long tnow = time(NULL);
		while (ev.betterThan(target_f, e.evaluation()) && (k <= r) && ((tnow - tini) < timelimit))
		{
			Solution<R>* s0 = &s.clone();
			Evaluation<M>* e0 = &e.clone();

			neighbors[k - 1]->exec(*s0, *e0, timelimit, target_f);
			if (ev.betterThan(*s0, s))
			{
				s = *s0;
				e = *e0;
				delete s0;
				delete e0;
				k = 1;
			}
			else
			{
				delete s0;
				delete e0;
				k = k + 1;
			}
			ev.evaluate(e, s);

			tnow = time(NULL);
		}

	}

private:
	vector<Heuristic<R, M>*> neighbors;
	Evaluator<R, M>& ev;
};

#endif /*RVND_HPP_*/
