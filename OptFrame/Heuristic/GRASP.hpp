#ifndef OPTFRAME_GRASP_HPP_
#define OPTFRAME_GRASP_HPP_

#include "../Heuristic.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class GRASP: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& evaluator;
	Heuristic<R, M>& h;
	InitialSolution<R>& initsol;
	unsigned int iterMax;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	GRASP(Evaluator<R, M>& _eval, InitialSolution<R>& _initsol, Heuristic<R, M>& _h, int _iterMax) :
		evaluator(_eval), initsol(_initsol), h(_h), iterMax(_iterMax)
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
			Solution<R>& s1 = initsol.generateSolution();
			Evaluation<M>& e1 = evaluator.evaluate(s1);

			h.exec(s1,e1,timelimit, target_f);

			if(evaluator.betterThan(e1,e))
			{
				s = s1;
				e = e1;
				cout << "GRASP iter "<<iter<<": ";
				e.print();
			}

			delete &s1;
			delete &e1;

			tnow = time(NULL);
			iter++;
		}
	}
};

#endif /*OPTFRAME_GRASP_HPP_*/
