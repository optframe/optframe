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

template<XSolution S, XEvaluation XMEv=MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
class MOILSLevels: public MultiObjILS<levelHistory, S, XMEv>
{

private:
	MOILSLPerturbation<S, XMEv>& p;
	int iterMax, levelMax;

public:

	//MOILSLevels(MultiEvaluator<S, XEv>& _mev, InitialPareto<S, XMEv>& _init_pareto, int _init_pop_size, MOLocalSearch<S, XEv>* _ls, RandGen& _rg, MOILSLPerturbation<S, XEv>& _p, int _iterMax, int _levelMax) :
   MOILSLevels(GeneralEvaluator<XMES, XMEv>& _mev, InitialPareto<S, XMEv>& _init_pareto, int _init_pop_size, MOLocalSearch<S, XEv>* _ls, RandGen& _rg, MOILSLPerturbation<S, XEv>& _p, int _iterMax, int _levelMax) :
			MultiObjILS<levelHistory, S, XMEv>(_mev, _init_pareto, _init_pop_size, _ls, _rg), p(_p), iterMax(_iterMax), levelMax(_levelMax)
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

	virtual void perturbation(S& s, XMEv& mev, const StopCriteria<XMEv>& stopCriteria, levelHistory& history) override
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

	virtual void acceptanceCriterion(const Pareto<S, XMEv>& pf, levelHistory& history)
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
		return (s == idComponent()) || (MultiObjSearch<S, XMEv>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MultiObjILS<levelHistory, S, XEv>::idComponent() << "MOILSLevels";
		return ss.str();
	}

};

}

#endif /*OPTFRAME_MOILSLevels_HPP_*/
