#ifndef OPTFRAME_ILSL_HPP_
#define OPTFRAME_ILSL_HPP_

#include <math.h>
#include <vector>

#include "IteratedLocalSearch.hpp"
#include "ILSLPerturbation.hpp"

typedef pair<pair<int, int> , pair<int, int> > levelHistory;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class IteratedLocalSearchLevels: public IteratedLocalSearch<levelHistory, R, M>
{
protected:
	Heuristic<R, M>& h;
	ILSLPerturbation<R, M>& p;
	int iterMax, levelMax;

public:

	IteratedLocalSearchLevels(Evaluator<R, M>& e, Heuristic<R, M>& _h, ILSLPerturbation<R, M>& _p, int _iterMax, int _levelMax) :
		IteratedLocalSearch<levelHistory, R, M> (e), h(_h), p(_p), iterMax(_iterMax), levelMax(_levelMax)
	{
	}

	virtual levelHistory& initializeHistory()
	{
		//cout << "initializeHistory()" << endl;
		pair<int, int> vars(0, 0);

		// IterMax e LevelMax
		pair<int, int> maxs(iterMax, levelMax);

		return *new levelHistory(vars, maxs);
	}

	virtual void localSearch(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		//cout << "localSearch(.)" << endl;
		h.exec(s, e, timelimit, target_f);
	}

	virtual void perturbation(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f, levelHistory& history)
	{
		//cout << "perturbation(.)" << endl;

		int iter = history.first.first;
		int level = history.first.second;
		int iterMax = history.second.first;
		//int levelMax = history.second.second;

		//cout << "level = " << level << " e iter = " << iter << endl;

		// nivel atual: 'level'
		p.perturb(s, e, timelimit, target_f, level);

		// Incrementa a iteracao
		iter++;

		if (iter >= iterMax)
		{
			iter = 0;
			level++;
			cout << "level " << level << ".." << endl;
		}

		// Atualiza o historico
		history.first.first = iter;
		history.first.second = level;
	}

	virtual Solution<R>& acceptanceCriterion(const Solution<R>& s1, const Solution<R>& s2, levelHistory& history)
	{
		//cout << "acceptanceCriterion(.)" << endl;

		if (IteratedLocalSearch<levelHistory, R, M>::evaluator.betterThan(s2, s1))
		{
			// =======================
			//   Melhor solucao: 's2'
			// =======================
			Evaluation<M>& e = IteratedLocalSearch<levelHistory, R, M>::evaluator.evaluate(s2);
			cout << "Best fo: " << e.evaluation();
			cout << " on [iter " << history.first.first << " of level " << history.first.second << "]" << endl;
			delete &e;

			// =======================
			//  Atualiza o historico
			// =======================
			// iter = 0
			history.first.first = 0;
			// level = 0
			history.first.second = 0;

			// =======================
			//    Retorna s2
			// =======================
			return s2.clone();
		}
		else
			return s1.clone();
	}

	virtual bool terminationCondition(levelHistory& history)
	{
		//cout << "terminationCondition(.)" << endl;
		int level = history.first.second;
		int levelMax = history.second.second;

		return (level >= levelMax);
	}
};

#endif /*OPTFRAME_ILSL_HPP_*/
