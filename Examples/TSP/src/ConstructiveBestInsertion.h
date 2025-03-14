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

#ifndef TSP_BEST_INSERTION_CONSTRUCTIVE_HPP_
#define TSP_BEST_INSERTION_CONSTRUCTIVE_HPP_

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Search/InitialSearch.hpp>
// #include "../../OptFrame/Util/TestSolution.hpp" // DEPRECATED

#include <stdlib.h>

#include <OptFrame/Core/RandGen.hpp>
#include <algorithm>
#include <list>

#include "Evaluator.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace std;

namespace TSP {

// class ConstructiveBestInsertion: public Constructive<SolutionTSP>
class ConstructiveBestInsertion : public InitialSearch<ESolutionTSP> {
 private:
  sref<ProblemInstance> pI;
  sref<GeneralEvaluator<ESolutionTSP>> eval;
  sref<RandGen> rg;

  static bool compare(const pair<double, pair<int, int>>& p1,
                      const pair<double, pair<int, int>>& p2) {
    return p1.first < p2.first;
  }

 public:
  ConstructiveBestInsertion(sref<ProblemInstance> pI,
                            sref<GeneralEvaluator<ESolutionTSP>> _eval,
                            sref<RandGen> _rg)
      : pI(pI),
        eval(_eval),
        rg(_rg)  // If necessary, add more parameters
  {
    // Put the rest of your code here
  };

  virtual ~ConstructiveBestInsertion() {}

  // std::optional<SolutionTSP> generateSolution(double timelimit) override
  std::pair<std::optional<ESolutionTSP>, SearchStatus> initialSearch(
      const StopCriteria<>& sosc) override {
    // cout << "Generating solution" << endl;
    RepTSP newRep;
    vector<bool> used(pI->n, false);

    int first = rg->rand(pI->n);

    newRep.push_back(first);
    used[first] = true;

    int second = rg->rand(pI->n);
    while (second == first) second = rg->rand(pI->n);

    newRep.push_back(second);
    used[second] = true;

    while (((int)newRep.size()) < pI->n) {
      vector<pair<double, pair<int, int>>> candidates;

      // cout << "BASE: " << newRep << endl;

      for (unsigned i = 0; i < used.size(); i++)
        if (!used[i])
          for (unsigned j = 0; j < newRep.size(); j++) {
            // city i will be in position j
            unsigned bj = j - 1;
            if (j == 0) bj = newRep.size() - 1;

            double cost = -(*pI->dist)(newRep[bj], newRep[j]);  // remove arc
            // cout << "Candidate: " << " i=" <<i << " j=" << j << " cost=" <<
            // cost << endl;
            cost += (*pI->dist)(newRep[bj], i);
            // cout << "Candidate: " << " i=" <<i << " j=" << j << " cost=" <<
            // cost << endl;
            cost += (*pI->dist)(i, newRep[j]);
            // cout << "Candidate: " << " i=" <<i << " j=" << j << " cost=" <<
            // cost << endl;
            candidates.push_back(make_pair(cost, make_pair(i, j)));
          }

      // cout << "before sort: " << newRep << endl;
      sort(candidates.begin(), candidates.end(), compare);

      int best_pos = candidates[0].second.second;
      int best_city = candidates[0].second.first;

      // CHECK

      /*
            cout << "Solution is now: " << newRep << endl;
            cout << "Best insertion is: city=" << best_city << " in position="
         << best_pos << " value=" << candidates[0].first << endl;
            */

      newRep.insert(newRep.begin() + best_pos, best_city);
      used[best_city] = true;

      /*

            int bef_pos = best_pos-1;
            int aft_pos = best_pos+1;
            if(best_pos==0)
                bef_pos = ((int)newRep.size())-1;
            if(best_pos==((int)newRep.size()))
                aft_pos = 0;

            double calcCost = -(*pI->dist)(newRep[bef_pos], newRep[aft_pos]);
            cout << "calcCost = -d(" << newRep[bef_pos] << "," <<
         newRep[aft_pos] << ") "; calcCost += (*pI->dist)(newRep[bef_pos],
         newRep[best_pos]); cout << "calcCost += d(" << newRep[bef_pos] << ","
         << newRep[best_pos] << ") "; calcCost += (*pI->dist)(newRep[best_pos],
         newRep[aft_pos]); cout << "calcCost += d(" << newRep[best_pos] << ","
         << newRep[aft_pos] << ") " << endl; cout << "Solution became: " <<
         newRep << endl;


            if(candidates[0].first != calcCost)
            {
                cout << "DIFFERENCE BETWEEN COSTS! Best: "<<candidates[0].first
         << " CALC: " << calcCost << endl; exit(1);
            }
            */

      // cout << "after sort: " << newRep << endl;
    }

    // return new CopySolution<RepTSP>(newRep);
    // return make_optional(SolutionTSP(newRep));
    EvaluationTSP etsp;
    ESolutionTSP se(SolutionTSP(newRep), etsp);
    eval->reevaluate(se);
    return make_pair(make_optional(se), SearchStatus::NO_REPORT);
  }
};

}  // namespace TSP

#endif /*TSP_BEST_INSERTION_CONSTRUCTIVE_HPP_*/
