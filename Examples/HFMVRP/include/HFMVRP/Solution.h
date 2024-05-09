#ifndef HFMVRP_SOLUTION_H_
#define HFMVRP_SOLUTION_H_

// C++
#include <utility>

//
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "ADS.h"
#include "Evaluation.h"
#include "Representation.h"

using namespace optframe;

namespace HFMVRP {

using SolutionHFMVRP = CopySolution<RepHFMVRP, AdsHFMVRP>;

using ESolutionHFMVRP = std::pair<SolutionHFMVRP, EvaluationHFMVRP>;

// adapter for VRP neighborhoods
vector<vector<int>>& localGetRoutes(const ESolutionHFMVRP& s) {
  vector<vector<int>>& v = const_cast<vector<vector<int>>&>(s.first.getR());
  return v;
};

}  // namespace HFMVRP
#endif /*HFMVRP_SOLUTION_H_*/
