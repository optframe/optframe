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

template<class H, XSolution S, XEvaluation XEv = Evaluation<>>
class IteratedLocalSearch: public ILS, public SingleObjSearch<S, XEv>
{
protected:
	Evaluator<S, XEv>& evaluator;
	Constructive<S>& constructive;

public:

	IteratedLocalSearch(Evaluator<S, XEv>& _evaluator, Constructive<S>& _constructive) :
			evaluator(_evaluator), constructive(_constructive)
	{
	}

	virtual ~IteratedLocalSearch()
	{
	}

	virtual H& initializeHistory() = 0;

	virtual void localSearch(pair<S, XEv>& se, SOSC& stopCriteria) = 0;

	virtual void perturbation(pair<S, XEv>& se, SOSC& stopCriteria, H& history) = 0;

	virtual bool acceptanceCriterion(const Evaluation<>& e1, const Evaluation<>& e2, H& history) = 0;

	virtual bool terminationCondition(H& history) = 0;

   pair<S, XEv> genPair(double timelimit)
   {
      std::optional<S> sStar = constructive.generateSolution(timelimit);
		XEv eStar = evaluator.evaluate(*sStar);
      return make_pair(*sStar, eStar); 
   }

	//pair<S, Evaluation<>>* search(SOSC& stopCriteria, const S* _s = nullptr, const Evaluation<>* _e = nullptr) override
   virtual std::optional<pair<S, XEv>> search(SOSC& stopCriteria, const std::optional<pair<S, XEv>> input = std::nullopt) override
	{
		cout << "ILS opt search(" << stopCriteria.target_f << "," << stopCriteria.timelimit << ")" << endl;

		Timer tnow;

      pair<S, XEv> star = input?*input:genPair(stopCriteria.timelimit);
		//S& sStar = star.first;
		Evaluation<>& eStar = star.second;

      /*
		//If solution is given it should contain an evaluation: TODO - Implement search with Solution
		if (input)
		{
			//(*sStar) = (*_s); // shouldn't this break memory?
			//(*eStar) = (*_e); // shouldn't this break memory?
         sStar = new S(input->first);
         eStar = new XEv(input->second);
		}
		else
		{
			sStar = new S(std::move(*constructive.generateSolution(stopCriteria.timelimit)));
			eStar = new Evaluation(evaluator.evaluate(*sStar));
		}
      */

		if (Component::information)
		{
			cout << "ILS::starting with FO:" << endl;
			eStar.print();
		}

		H* history = &initializeHistory();

		// 's0' <- GenerateSolution
		// 's*' <- localSearch 's'

		if (Component::information)
			cout << "ILS::performing first local search" << endl;
		SOSC stopCriteriaLS = stopCriteria;
		stopCriteriaLS.updateTimeLimit(tnow.now());
		localSearch(star, stopCriteriaLS);
		if (Component::information)
			cout << "ILS::finished first local search" << endl;

		cout << "ILS optimized starts: ";
		eStar.print();

		do
		{
         pair<S, XEv> p1 = star; // copy (how to automatically invoke clone?)
			//S s1(sStar); // copy (should clone?)
			//Evaluation<> e1(eStar); // copy (should clone?)
         
			SOSC stopCriteriaPert = stopCriteria;
			stopCriteriaPert.updateTimeLimit(tnow.now());
			perturbation(p1, stopCriteriaPert, *history);

			SOSC stopCriteriaLS2 = stopCriteria;
			stopCriteriaLS2.updateTimeLimit(tnow.now());
			localSearch(p1, stopCriteriaLS2);

         // Should update evaluation eStar? Why?
			//(*eStar) = evaluator.evaluate(*sStar);

			//bool improvement = acceptanceCriterion(e1, *eStar, *history);
         bool improvement = acceptanceCriterion(p1.second, star.second, *history);

			if (improvement)
			{
				//(*eStar) = e1;
				//(*sStar) = s1;
            star = p1; // copy, or should somehow use clone?
            // TODO: should probably move here to enhance performance (try to benchmark before!!)
			}

		} while (evaluator.betterThan(stopCriteria.target_f, eStar.evaluation()) && !terminationCondition(*history) && ((tnow.now()) < stopCriteria.timelimit));

		if (evaluator.betterThan(eStar.evaluation(), stopCriteria.target_f))
			cout << "ILS exit by target_f: " << eStar.evaluation() << " better than " << stopCriteria.target_f << endl;

		//pair<S, Evaluation<>>* pairToReturn = new pair<S, Evaluation<>>(make_pair(std::move(*sStar), std::move(*eStar)));
      
		//delete eStar;
		//delete sStar;

		delete history; // why do we need this?

		//return std::optional<pair<S,XEv>>(*pairToReturn); // TODO: prevent loss
      return std::optional<pair<S,XEv>>(star);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<S, XEv>::idComponent() << ":" << ILS::family();
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_ILS_HPP_*/
