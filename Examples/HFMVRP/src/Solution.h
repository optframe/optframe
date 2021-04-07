#ifndef HFMVRP_SOLUTION_H_
#define HFMVRP_SOLUTION_H_

#include <OptFrame/Solution.hpp>
#include <OptFrame/Solutions/CopySolution.hpp>

#include "ADS.h"
#include "Evaluation.h"
#include "Representation.h"

using namespace optframe;

namespace HFMVRP {

typedef CopySolution<RepHFMVRP, AdsHFMVRP> SolutionHFMVRP;

typedef pair<SolutionHFMVRP, EvaluationHFMVRP> ESolutionHFMVRP;

// adapter for VRP neighborhoods
vector<vector<int>>&
localGetRoutes(const ESolutionHFMVRP& s)
{
   vector<vector<int>>& v = const_cast<vector<vector<int>>&>(s.first.getR());
   return v;
};

}
#endif /*HFMVRP_SOLUTION_H_*/
