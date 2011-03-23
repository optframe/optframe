#ifndef OPTFRAME_BASICILS_HPP_
#define OPTFRAME_BASICILS_HPP_

#include <math.h>
#include <vector>

#include "IteratedLocalSearch.hpp"
#include "BasicILSPerturbation.hpp"

typedef int BasicHistory;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class BasicIteratedLocalSearch: public IteratedLocalSearch<BasicHistory, R, M>
{
protected:
	Heuristic<R, M>& h;
	BasicILSPerturbation<R, M>& p;
	int iterMax;

public:

	BasicIteratedLocalSearch(Evaluator<R, M>& e, Heuristic<R, M>& _h, BasicILSPerturbation<R, M>& _p, int _iterMax) :
		IteratedLocalSearch<BasicHistory, R, M> (e), h(_h), p(_p), iterMax(_iterMax)
	{
	}

	virtual BasicHistory& initializeHistory()
	{
		int& iter = * new int;
		iter = 0;

		return iter;
	}

	virtual void localSearch(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		h.exec(s, e, timelimit, target_f);
	}

	virtual void perturbation(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f, BasicHistory& history)
	{
		int iter = history;

		p.perturb(s, e, timelimit, target_f);

		// Incrementa a iteracao
		iter++;

		// Atualiza o historico
		history = iter;
	}

	virtual Solution<R>& acceptanceCriterion(const Solution<R>& s1, const Solution<R>& s2, BasicHistory& history)
	{
		if (IteratedLocalSearch<BasicHistory, R, M>::evaluator.betterThan(s2, s1))
		{
			// =======================
			//   Melhor solucao: 's2'
			// =======================
			Evaluation<M>& e = IteratedLocalSearch<BasicHistory, R, M>::evaluator.evaluate(s2);
			cout << "Best fo: " << e.evaluation();
			cout << " on [iter " << history << "]" << endl;
			delete &e;

			// =======================
			//  Atualiza o historico
			// =======================
			// iter = 0
			history = 0;

			// =======================
			//    Retorna s2
			// =======================
			return s2.clone();
		}
		else
			return s1.clone();
	}

	virtual bool terminationCondition(BasicHistory& history)
	{
		int iter = history;

		return (iter >= iterMax);
	}
};

#endif /*OPTFRAME_BASICILS_HPP_*/
