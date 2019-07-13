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

#ifndef OPTFRAME_MOILSLevels_HPP_
#define OPTFRAME_MOILSLevels_HPP_

#include <algorithm>

#include "MultiObjILS.hpp"
#include "MOILSLevels.hpp"


namespace optframe
{

typedef pair<pair<int, int>, pair<int, int> > levelHistory;

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MOILSLevels: public MultiObjILS<levelHistory, R, ADS>
{

private:
	MOILSLPerturbation<R, ADS>& p;
	int iterMax, levelMax;

public:

	MOILSLevels(MultiEvaluator<R, ADS>& _mev, InitialPareto<R, ADS>& _init_pareto, int _init_pop_size, MOLocalSearch<R, ADS>* _ls, RandGen& _rg, MOILSLPerturbation<R, ADS>& _p, int _iterMax, int _levelMax) :
			MultiObjILS<levelHistory, R, ADS>(_mev, _init_pareto, _init_pop_size, _ls, _rg), p(_p), iterMax(_iterMax), levelMax(_levelMax)
	{

	}

	virtual ~MOILSLevels()
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

	virtual void perturbation(Solution<R, ADS>& s, MultiEvaluation& mev, MOSC& stopCriteria, levelHistory& history)
	{
		//cout << "perturbation(.)" << endl;

		int iter = history.first.first;
		int level = history.first.second;
		int iterMax = history.second.first;
		//int levelMax = history.second.second;

		//cout << "level = " << level << " e iter = " << iter << endl;

		// nivel atual: 'level'
		p.perturb(s, mev, stopCriteria, level);

		// Incrementa a iteracao
		iter++;

		if (Component::debug)
			cout << "MOILSL::iter " << iter << endl;

		if (iter >= iterMax)
		{
			iter = 0;
			level++;
			if (Component::information)
				cout << "MOILSL::level " << level << ".." << endl;
		}

		// Atualiza o historico
		history.first.first = iter;
		history.first.second = level;
	}

	virtual void acceptanceCriterion(const Pareto<R, ADS>& pf, levelHistory& history)
	{
		if (pf.getNewNonDominatedSolutionsStatus())
		{
			cout << "New Pareto size: is " << pf.size();
			cout << " on [iter " << history.first.first << " of level " << history.first.second << "]"<<endl;
			// =======================
			//  Atualiza o historico
			// =======================
			// iter = 0
			history.first.first = 0;
			// level = 0
			history.first.second = 0;
		}
	}

	virtual bool terminationCondition(levelHistory& history)
	{
		int level = history.first.second;
		int levelMax = history.second.second;

		return (level >= levelMax);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (MultiObjSearch<R, ADS>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MultiObjILS<levelHistory, R, ADS>::idComponent() << "MOILSLevels";
		return ss.str();
	}

};

}

#endif /*OPTFRAME_MOILSLevels_HPP_*/
