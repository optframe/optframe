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

#ifndef OPTFRAME_MULTIOBJECTIVEILS_HPP_
#define OPTFRAME_MULTIOBJECTIVEILS_HPP_

#include <algorithm>

#include "../../../MultiObjSearch.hpp"
#include "../../../NSSeq.hpp"
#include "../../../Pareto.hpp"
#include "../../../MultiEvaluator.hpp"

#include "MOILS.h"

namespace optframe
{

template<class H, class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MultiObjILS: public MOILS, public MultiObjSearch<R, ADS>
{

private:
	InitialPareto<R, ADS>& init_pareto;
	int init_pop_size;
	MOLocalSearch<R, ADS>* ls;
	paretoManager<R, ADS> pMan;
	RandGen& rg;

public:

	MultiObjILS(MultiEvaluator<R, ADS>& _mev, InitialPareto<R, ADS>& _init_pareto, int _init_pop_size, MOLocalSearch<R, ADS>* _ls, RandGen& _rg) :
			init_pareto(_init_pareto), init_pop_size(_init_pop_size), ls(_ls), pMan(paretoManager<R, ADS>(_mev)), rg(_rg)
	{

	}

	virtual ~MultiObjILS()
	{
	}

	virtual H& initializeHistory() = 0;

	virtual void perturbation(Solution<R, ADS>& s, MultiEvaluation& e, MOSC& stopCriteria, H& history) = 0;

	virtual void acceptanceCriterion(const Pareto<R, ADS>& pf, H& history) = 0;

	virtual bool terminationCondition(H& history) = 0;

	virtual Pareto<R, ADS>* search(MOSC& stopCriteria, Pareto<R, ADS>* _pf = nullptr)
	{
		Timer tnow;
		Pareto<R, ADS> x_e;
		cout << "exec: MOILS (tL:" << stopCriteria.timelimit << ")" << endl;

		if (_pf == nullptr)
		{
			if (Component::information)
				cout << "Creating initial population using a initial pareto method:" << init_pop_size << endl;

			if (tnow.now() < stopCriteria.timelimit)
				x_e = std::move(init_pareto.generatePareto(init_pop_size, stopCriteria.timelimit - tnow.now()));

			if (Component::information)
				cout << "Population generated with " << x_e.size() << " individuals!" << endl;
		}
		else
		{
			assert(_pf->size() > 0);
			if (Component::information)
				cout << "Extracting Pareto Front given as parameter..." << endl;

			x_e = std::move(*_pf); //check this move with AIIIGOR todo

			if (Component::information)
				cout << "Extracting PF contains " << x_e.size() << " individuals." << endl;
		}

//		vector<bool> visited;
//		for (int ind = 0; ind < x_e.size(); ind++)
//			visited.push_back(false);

		H* history = &initializeHistory();

		while (tnow.now() < stopCriteria.timelimit && !terminationCondition(*history))
		{
			int ind = rg.rand(x_e.size());
//			while (visited[ind])
//				ind = rg.rand(x_e.size());
//			visited[ind] = true;

			Solution<R, ADS> rS = x_e.getNonDominatedSol(ind);
			MultiEvaluation rMev = x_e.getIndMultiEvaluation(ind);

			MOSC stopCriteriaPert;
			stopCriteriaPert.timelimit = stopCriteria.timelimit;
			perturbation(rS, rMev, stopCriteriaPert, *history);

			//Try to add the neighbor solution that was obtained from the perturbation
			pMan.addSolutionWithMEV(x_e, rS, rMev);

			MOSC stopCriteriaLS;
			stopCriteriaLS.timelimit = stopCriteria.timelimit;
			ls->exec(x_e, rS, rMev, pMan, stopCriteriaLS);


			acceptanceCriterion(x_e, *history);
			x_e.setNewNonDominatedSolutionsStatus(false);

			//This is undoubtedly not working properly - TODO
			//Visited vize should be updated. It may have been deleted
//			bool allVisited = true;
//			for (int v = 0; v < D.size(); v++)
//			{
//				if (!visited[v])
//				{
//					allVisited = false;
//					break;
//				}
//			}
//
//			if (allVisited)
//				for (int v = 0; v < D.size(); v++)
//					visited[v] = false;

		}

		Pareto<R, ADS>* pReturn = new Pareto<R, ADS>(std::move(x_e));

		//checking possible dominance problems -- TODO - Remove for a faster code
		pMan.checkDominance(*pReturn);

		cout << "MOILS finished with " << pReturn->size() << " non-dominated solutions." << endl;

		delete history;
		return pReturn;
	}

};

}

#endif /*OPTFRAME_MULTIOBJECTIVEILS_HPP_*/
