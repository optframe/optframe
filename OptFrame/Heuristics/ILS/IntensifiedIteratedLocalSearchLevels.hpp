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

#ifndef OPTFRAME_IILSL_HPP_
#define OPTFRAME_IILSL_HPP_

#include <math.h>
#include <vector>

#include "IntensifiedIteratedLocalSearch.hpp"
#include "ILSLPerturbation.hpp"
#include "Intensification.hpp"

#include "../../LocalSearch.hpp"

namespace optframe
{

typedef pair<pair<int, int> , pair<int, int> > levelHistory;

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class IntensifiedIteratedLocalSearchLevels: public IntensifiedIteratedLocalSearch<levelHistory, R, ADS, DS >
{
protected:
	LocalSearch<R, ADS, DS>& ls;
	Intensification<R, ADS, DS>& h2;
	ILSLPerturbation<R, ADS, DS>& p;
	int iterMax, levelMax;

public:

	IntensifiedIteratedLocalSearchLevels(Evaluator<R, ADS, DS>& e, Constructive<R, ADS>& constructive, LocalSearch<R, ADS, DS>& _ls, Intensification<R, ADS, DS>& _h2, ILSLPerturbation<R, ADS, DS>& _p, int _iterMax, int _levelMax) :
		IntensifiedIteratedLocalSearch<levelHistory, R, ADS, DS > (e, constructive), ls(_ls), h2(_h2), p(_p)
	{
		iterMax = _iterMax;
		levelMax = _levelMax;
	}

	virtual ~IntensifiedIteratedLocalSearchLevels()
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

	virtual void intensification(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f, levelHistory& history)
	{
		h2.addSolution(s);

		if (history.first.first == history.second.first-1) //TODO intensification applied only at last iteration of each level
		{
			Solution<R, ADS>& s1 = h2.search(s);

			Evaluator<R, ADS, DS> & ev = this->getEvaluator();
			Evaluation<DS>& s1_e = ev.evaluate(s1);

			if (ev.betterThan(s1_e, e))
			{
				e = s1_e;
				s = s1;
				h2.addSolution(s);
			}

			delete &s1;
		}

	}

	virtual void localSearch(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f)
	{
		//cout << "localSearch(.)" << endl;
		ls.exec(s, e, timelimit, target_f);
	}

	virtual void perturbation(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f, levelHistory& history)
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
		if (IntensifiedIteratedLocalSearch<levelHistory, R, ADS, DS >::evaluator.betterThan(s2, s1))
		{
			// =======================
			//   Melhor solucao: 's2'
			// =======================
			Evaluation<DS>& e = IntensifiedIteratedLocalSearch<levelHistory, R, ADS, DS >::evaluator.evaluate(s2);
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

			h2.addSolution(s2);

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
		ss << IntensifiedIteratedLocalSearch<levelHistory, R, ADS, DS >::idComponent() << "iils";
		return ss.str();

	}
};

}

#endif /*OPTFRAME_IILSL_HPP_*/
