// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef OPTFRAME_ILS_HPP_
#define OPTFRAME_ILS_HPP_

#include <math.h>
#include <vector>

#include "../../Constructive.hpp"
#include "../../SingleObjSearch.hpp"
#include "../../Evaluator.hpp"
#include "../../Timer.hpp"

#include "ILS.h"

namespace optframe
{

template<class H, class R, class ADS = OPTFRAME_DEFAULT_ADS>
class IteratedLocalSearch: public ILS, public SingleObjSearch<R, ADS>
{
protected:
	Evaluator<R, ADS>& evaluator;
	Constructive<R, ADS>& constructive;

public:

	IteratedLocalSearch(Evaluator<R, ADS>& _evaluator, Constructive<R, ADS>& _constructive) :
			evaluator(_evaluator), constructive(_constructive)
	{
	}

	virtual ~IteratedLocalSearch()
	{
	}

	virtual H& initializeHistory() = 0;

	virtual void localSearch(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria) = 0;

	virtual void perturbation(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria, H& history) = 0;

	virtual bool acceptanceCriterion(const Evaluation& e1, const Evaluation& e2, H& history) = 0;

	virtual bool terminationCondition(H& history) = 0;

	pair<Solution<R, ADS>, Evaluation>* search(SOSC& stopCriteria, const Solution<R, ADS>* _s = nullptr, const Evaluation* _e = nullptr)
	{
		cout << "ILS opt search(" << stopCriteria.target_f << "," << stopCriteria.timelimit << ")" << endl;

		Timer tnow;

		Solution<R, ADS>* sStar = nullptr;
		Evaluation* eStar = nullptr;

		//If solution is given it should contain an evaluation: TODO - Implement search with Solution
		if (_s != nullptr)
		{
			(*sStar) = (*_s);
			(*eStar) = (*_e);
		}
		else
		{
			sStar = new Solution<R, ADS>(std::move(*constructive.generateSolution(stopCriteria.timelimit)));
			eStar = new Evaluation(evaluator.evaluateSolution(*sStar));
		}

		if (Component::information)
		{
			cout << "ILS::starting with FO:" << endl;
			eStar->print();
		}

		H* history = &initializeHistory();

		// 's0' <- GenerateSolution
		// 's*' <- localSearch 's'

		if (Component::information)
			cout << "ILS::performing first local search" << endl;
		SOSC stopCriteriaLS = stopCriteria;
		stopCriteriaLS.updateTimeLimit(tnow.now());
		localSearch(*sStar, *eStar, stopCriteriaLS);
		if (Component::information)
			cout << "ILS::finished first local search" << endl;

		cout << "ILS optimized starts: ";
		eStar->print();

		do
		{
			Solution<R, ADS> s1(*sStar);
			Evaluation e1(*eStar);

			SOSC stopCriteriaPert = stopCriteria;
			stopCriteriaPert.updateTimeLimit(tnow.now());
			perturbation(s1, e1, stopCriteriaPert, *history);

			SOSC stopCriteriaLS2 = stopCriteria;
			stopCriteriaLS2.updateTimeLimit(tnow.now());
			localSearch(s1, e1, stopCriteriaLS2);

			(*eStar) = evaluator.evaluateSolution(*sStar);
			bool improvement = acceptanceCriterion(e1, *eStar, *history);

			if (improvement)
			{
				(*eStar) = e1;
				(*sStar) = s1;
			}

		} while (evaluator.betterThan(stopCriteria.target_f, eStar->evaluation()) && !terminationCondition(*history) && ((tnow.now()) < stopCriteria.timelimit));

		if (evaluator.betterThan(eStar->evaluation(), stopCriteria.target_f))
			cout << "ILS exit by target_f: " << eStar->evaluation() << " better than " << stopCriteria.target_f << endl;

		pair<Solution<R, ADS>, Evaluation>* pairToReturn = new pair<Solution<R, ADS>, Evaluation>(make_pair(std::move(*sStar), std::move(*eStar)));

		delete eStar;
		delete sStar;

		delete history;

		return pairToReturn;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << ":" << ILS::family();
		//ss << SingleObjSearch<R, ADS>::idComponent() << ILS::family << "IteratedLocalSearch:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_ILS_HPP_*/
