// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_ILSL_HPP_
#define OPTFRAME_ILSL_HPP_

#include <math.h>
#include <vector>

#include "IteratedLocalSearch.hpp"
#include "ILSLPerturbation.hpp"

typedef pair<pair<int, int> , pair<int, int> > levelHistory;

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class IteratedLocalSearchLevels: public IteratedLocalSearch<levelHistory, R, ADS, M>
{
protected:
	LocalSearch<R, ADS, M>& ls;
	ILSLPerturbation<R, ADS, M>& p;
	int iterMax, levelMax;

public:

	IteratedLocalSearchLevels(Evaluator<R, ADS, M>& e, Constructive<R, ADS>& constructive, LocalSearch<R, ADS, M>& _ls, ILSLPerturbation<R, ADS, M>& _p, int _iterMax, int _levelMax) :
		IteratedLocalSearch<levelHistory, R, ADS, M> (e, constructive), ls(_ls), p(_p), iterMax(_iterMax), levelMax(_levelMax)
	{
	}

	virtual ~IteratedLocalSearchLevels()
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

	virtual void localSearch(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		//cout << "localSearch(.)" << endl;
		ls.exec(s, e, timelimit, target_f);
	}

	virtual void perturbation(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f, levelHistory& history)
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

	virtual Solution<R, ADS>& acceptanceCriterion(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2, levelHistory& history)
	{
		//cout << "acceptanceCriterion(.)" << endl;

		if (IteratedLocalSearch<levelHistory, R, ADS, M>::evaluator.betterThan(s2, s1))
		{
			// =======================
			//   Melhor solucao: 's2'
			// =======================
			Evaluation<M>& e = IteratedLocalSearch<levelHistory, R, ADS, M>::evaluator.evaluate(s2);
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

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << IteratedLocalSearch<levelHistory, R, ADS, M>::idComponent() << "ilsl";
		return ss.str();

	}
};

#endif /*OPTFRAME_ILSL_HPP_*/
