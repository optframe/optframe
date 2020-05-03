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

#ifndef TSP_INITIALSOLUTION_Random_HPP_
#define TSP_INITIALSOLUTION_Random_HPP_

#include "../../OptFrame/Constructive.hpp"
//#include "../../OptFrame/Util/TestSolution.hpp" // DEPRECATED

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"
#include "../../OptFrame/RandGen.hpp"
#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

namespace TSP
{

//class RandomInitialSolutionTSP: public Constructive<SolutionTSP>
class RandomInitialSolutionTSP: public InitialSearch<ESolutionTSP, EvaluationTSP>
{
private:
	ProblemInstance* pI;
   GeneralEvaluator<ESolutionTSP>& eval;
	RandGen& rg;

	// Your private vars

public:

	RandomInitialSolutionTSP(ProblemInstance* pI, GeneralEvaluator<ESolutionTSP>& _eval, RandGen& _rg) :
      eval(_eval), rg(_rg) // If necessary, add more parameters
	{
		this->pI = pI;
		// Put the rest of your code here
	};

	virtual ~RandomInitialSolutionTSP()
	{
	}

	//std::optional<SolutionTSP> generateSolution(double timelimit) override
   std::optional<ESolutionTSP> initialSearch(const StopCriteria<>& sosc) override
	{
		RepTSP newRep(pI->n);

		vector<int> r(pI->n);
		for(unsigned int i=0;i<r.size();i++)
			r[i]=i;

		rg.shuffle(r); // shuffle elements of r

		for(unsigned int i=0;i<newRep.size();i++)
			newRep[i] = r[i];

		//return new CopySolution<RepTSP>(newRep);
      //return make_optional(SolutionTSP(newRep));
      ESolutionTSP se(newRep, EvaluationTSP());
      eval.reevaluate(se);
      return make_optional(se);
	}

};

}

#endif /*TSP_INITIALSOLUTION_Random_HPP_*/
