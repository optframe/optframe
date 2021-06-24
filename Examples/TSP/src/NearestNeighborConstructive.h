// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef TSP_NN_CONSTRUCTIVE_HPP_
#define TSP_NN_CONSTRUCTIVE_HPP_

#include <OptFrame/Constructive.hpp>
#include <OptFrame/InitialSearch.hpp>
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

namespace TSP {

//class NearestNeighborConstructive: public Constructive<SolutionTSP>
class NearestNeighborConstructive : public InitialSearch<ESolutionTSP, EvaluationTSP>
{
private:
   sref<ProblemInstance> pI;
   sref<GeneralEvaluator<ESolutionTSP>> eval;
   sref<RandGen> rg;

   static bool compare(const pair<double, int>& p1, const pair<double, int>& p2)
   {
      return p1.first < p2.first;
   }

public:
   NearestNeighborConstructive(sref<ProblemInstance> pI, sref<GeneralEvaluator<ESolutionTSP>> _eval, sref<RandGen> _rg)
     : pI(pI)
     , eval(_eval)
     , rg(_rg) // If necessary, add more parameters
     {
        // Put the rest of your code here
     };

   virtual ~NearestNeighborConstructive()
   {
   }

   //std::optional<SolutionTSP> generateSolution(double timelimit) override
   std::pair<std::optional<ESolutionTSP>, SearchStatus> initialSearch(const StopCriteria<>& sosc) override
   {
      //cout << "Generating solution" << endl;
      RepTSP newRep;
      vector<bool> used(pI->n, false);

      int first = rg->rand(pI->n);

      newRep.push_back(first);
      used[first] = true;
      //cout << "first is " << first << endl;

      while (((int)newRep.size()) < pI->n - 1) {
         vector<pair<double, int>> candidates;

         for (unsigned i = 0; i < used.size(); i++)
            if (!used[i])
               candidates.push_back(make_pair((*pI->dist)(i, newRep.at(newRep.size() - 1)), i));

         //cout << "before sort: " << newRep << endl;
         sort(candidates.begin(), candidates.end(), compare);

         newRep.push_back(candidates[0].second);
         used[candidates[0].second] = true;

         //cout << "after sort: " << newRep << endl;
      }

      // add last
      for (unsigned i = 0; i < used.size(); i++)
         if (!used[i]) {
            newRep.push_back(i);
            used[i] = true;
            break;
         }

      //return new CopySolution<RepTSP>(newRep);
      //return make_optional(SolutionTSP(newRep));
      EvaluationTSP etsp;
      ESolutionTSP se(SolutionTSP(newRep), etsp);
      eval->reevaluate(se);
      return make_pair(make_optional(se), SearchStatus::NO_REPORT);
   }
};

}

#endif /*TSP_NN_CONSTRUCTIVE_HPP_*/
