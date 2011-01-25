#ifndef OPTFRAME_IILSL_HPP_
#define OPTFRAME_IILSL_HPP_

#include <math.h>
#include <vector>

#include "IntensifiedIteratedLocalSearch.hpp"
#include "ILSLPerturbation.hpp"

typedef pair<pair<int, int> , pair<int, int> > levelHistory;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class IntensifiedIteratedLocalSearchLevels: public IntensifiedIteratedLocalSearch<levelHistory, R, M>
{
protected:
	Heuristic<R, M>& h;
	Heuristic<R, M>& h2;
	ILSLPerturbation<R, M>& p;
	int iterMax, levelMax;
	Population<R> pool;
	int maxPool;

public:

	IntensifiedIteratedLocalSearchLevels(Evaluator<R, M>& e, Heuristic<R, M>& _h, Heuristic<R, M>& _h2, ILSLPerturbation<R, M>& _p, int _iterMax,
			int _levelMax) :
		h(_h), h2(_h2), p(_p), iterMax(_iterMax), levelMax(_levelMax), IntensifiedIteratedLocalSearch<levelHistory, R, M> (e)
	{
		maxPool = 30;
	}

	virtual levelHistory& initializeHistory()
	{
		//cout << "initializeHistory()" << endl;
		pair<int, int> vars(0, 0);

		// IterMax e LevelMax
		pair<int, int> maxs(iterMax, levelMax);

		return *new levelHistory(vars, maxs);
	}

	virtual void intensification(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f, levelHistory& history)
	{
		int applyLevel = history.first.second % 5;
		if ((history.first.first == 0) && (applyLevel == 0) && pool.size() >= 2)
		{

			Population<R>& v_pr = h2.search(pool);

			int best_i = 0;

			Evaluator<R, M>& ev = this->getEvaluator();

			for (int i = 1; i < v_pr.size(); i++)
				if (ev.betterThan(v_pr.at(i), v_pr.at(best_i)))
					best_i = i;

			Evaluation<M>& best_e = ev.evaluate(v_pr.at(best_i));

			if (ev.betterThan(best_e, e))
			{
				e = best_e;
				s = v_pr.at(best_i);
			}

			delete &v_pr;
		}

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
		if (pool.size() == 0)
			pool.push_back(s1.clone());

		if (IntensifiedIteratedLocalSearch<levelHistory, R, M>::evaluator.betterThan(s2, s1))
		{
			// =======================
			//   Melhor solucao: 's2'
			// =======================
			Evaluation<M>& e = IntensifiedIteratedLocalSearch<levelHistory, R, M>::evaluator.evaluate(s2);
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

			//Inseri a Solucao Otima"LOCAL" no pool - CRITERIO DE DIVERSIFICAO !! N TEM MTAS SOLUCOES- POE TDS
			if (pool.size() < maxPool)
				pool.push_back(s2.clone());

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

#endif /*OPTFRAME_IILSL_HPP_*/
