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

#ifndef TSP_NN_CONSTRUCTIVE_HPP_
#define TSP_NN_CONSTRUCTIVE_HPP_

#include <OptFrame/Constructive.hpp>
//#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"
#include <OptFrame/RandGen.hpp>
#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

namespace TSP
{

//class NearestNeighborConstructive: public Constructive<SolutionTSP>
class NearestNeighborConstructive: public InitialSearch<ESolutionTSP, EvaluationTSP>
{
private:
	ProblemInstance* pI;
   GeneralEvaluator<ESolutionTSP>& eval;
	RandGen& rg;

    static bool compare(const pair<double, int>& p1, const pair<double, int>& p2)
    {
        return p1.first < p2.first;
    }

public:

	NearestNeighborConstructive(ProblemInstance* pI, GeneralEvaluator<ESolutionTSP>& _eval, RandGen& _rg) :
      eval(_eval), rg(_rg) // If necessary, add more parameters
	{
		this->pI = pI;
		// Put the rest of your code here
	};

	virtual ~NearestNeighborConstructive()
	{
	}

	//std::optional<SolutionTSP> generateSolution(double timelimit) override
   std::optional<ESolutionTSP> initialSearch(const StopCriteria<>& sosc) override
	{
        //cout << "Generating solution" << endl;
		RepTSP newRep;
        vector<bool> used(pI->n, false);

        int first = rg.rand(pI->n);

        newRep.push_back(first);
        used[first] = true;
        //cout << "first is " << first << endl;

        while(((int)newRep.size()) < pI->n-1)
        {
            vector<pair<double, int> > candidates;

            for(unsigned i=0; i<used.size(); i++)
                if(!used[i])
                    candidates.push_back( make_pair((*pI->dist)(i, newRep.at(newRep.size()-1)) , i) );


            //cout << "before sort: " << newRep << endl;
            sort(candidates.begin(), candidates.end(), compare);

            newRep.push_back(candidates[0].second);
            used[candidates[0].second] = true;

            //cout << "after sort: " << newRep << endl;
        }

        // add last
        for(unsigned i=0; i<used.size(); i++)
            if(!used[i])
            {
                    newRep.push_back(i);
                    used[i] = true;
                    break;
            }

		//return new CopySolution<RepTSP>(newRep);
      //return make_optional(SolutionTSP(newRep));
      EvaluationTSP etsp;
      ESolutionTSP se(SolutionTSP(newRep), etsp);
      eval.reevaluate(se);
      return make_optional(se);
	}

};

}

#endif /*TSP_NN_CONSTRUCTIVE_HPP_*/
