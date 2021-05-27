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

template<class H, XESolution XES, XEvaluation XEv = Evaluation<>>
class IteratedLocalSearch: public ILS, public SingleObjSearch<XES>, public ITrajectory<XES>
{
protected:
	sref<GeneralEvaluator<XES, XEv>> evaluator;
	//Constructive<S>& constructive;
   sref<InitialSearch<XES, XEv>> constructive;

public:

	IteratedLocalSearch(sref<GeneralEvaluator<XES, XEv>> _evaluator, sref<InitialSearch<XES, XEv>> _constructive) :
			evaluator(_evaluator), constructive(_constructive)
	{
	}

	virtual ~IteratedLocalSearch()
	{
	}

	virtual sref<H> initializeHistory() = 0;

	virtual void localSearch(XES& se, const StopCriteria<XEv>& stopCriteria) = 0;

	virtual void perturbation(XES& se, const StopCriteria<XEv>& stopCriteria, sref<H> history) = 0;

	virtual bool acceptanceCriterion(const Evaluation<>& e1, const Evaluation<>& e2, sref<H> history) = 0;

	virtual bool terminationCondition(sref<H> history) = 0;

   // default search method (no initial solution passed)
   SearchOutput<XES> search(const StopCriteria<XEv>& stopCriteria) override
	{
      if(Component::information)
         std::cout << "ILS opt search(" << stopCriteria.timelimit << ")" << std::endl;
      //
      op<XES> star; // TODO: receive on 'searchBy'

      if(Component::debug)
         std::cout << "ILS::build initial solution" << std::endl;

      star = star?:constructive->initialSearch(stopCriteria).first;
      if(!star)
         return SearchStatus::NO_SOLUTION;

		Evaluation<>& eStar = star->second;
		if (Component::information)
		{
			std::cout << "ILS::starting with evaluation:" << std::endl;
			eStar.print();
		}

      return searchBy(*star, *star, stopCriteria);
   }

   // for ILS: incumbent is always derived from star, ignoring 'incumbent'
   virtual SearchOutput<XES> searchBy(
     XES& star,
     XES&,
     const StopCriteria<XEv>& stopCriteria)
   {
      if(Component::information)
         std::cout << "ILS opt searchBy(" << stopCriteria.timelimit << ")" << std::endl;

		Evaluation<>& eStar = star.second;
		if (Component::information)
		{
			std::cout << "ILS::starting with evaluation:" << std::endl;
			eStar.print();
		}

		sref<H> history = initializeHistory();

		// 's0' <- GenerateSolution
		// 's*' <- localSearch 's'

		if (Component::information)
			std::cout << "ILS::performing first local search" << std::endl;
      localSearch(star, stopCriteria);

		if (Component::information)
			std::cout << "ILS::finished first local search" << std::endl;

      if (Component::information) {
         std::cout << "ILS optimized starts: ";
         eStar.print();
      }

		do
		{
         XES p1 = star; // derive new incumbent solution (copy-based solution, for generality)
			perturbation(p1, stopCriteria, *history);
			localSearch(p1, stopCriteria);
         bool improvement = acceptanceCriterion(p1.second, star.second, history);
			if (improvement)
            star = p1; // copy-based
         std::cout << "SHOULD STOP?" << std::endl;
		}
      while (!terminationCondition(history) && !stopCriteria.shouldStop(star.second)); 

      if(!stopCriteria.target_f.outdated) {
         if(Component::debug)
            std::cout << "ILS will compare(" << eStar.outdated << ";" << stopCriteria.target_f.outdated << ")" << std::endl;
         if (evaluator->betterStrict(eStar, stopCriteria.target_f))
         {
            cout << "ILS exit by target_f: " << eStar.evaluation() << " better than " << stopCriteria.target_f.evaluation() << endl;
            cout << "isMin: " << eStar.isMini << endl;
         }
      }
      
      return {SearchStatus::NO_REPORT, star};
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<XES>::idComponent() << ":" << ILS::family();
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_ILS_HPP_*/
