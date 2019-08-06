#ifndef HFMVRP_SOLUTION_H_
#define HFMVRP_SOLUTION_H_

#include "../../OptFrame/Solution.hpp"
#include "../../OptFrame/Solutions/CopySolution.hpp"

#include "Representation.h"
#include "ADS.h"

using namespace optframe;

namespace HFMVRP{
typedef CopySolution<RepHFMVRP, AdsHFMVRP> SolutionHFMVRP;
}
#endif /*HFMVRP_SOLUTION_H_*/

