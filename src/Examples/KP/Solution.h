#ifndef KP_SOLUTION_H_
#define KP_SOLUTION_H_

#include "../../OptFrame/Solution.hpp"
#include "../../OptFrame/Solutions/CopySolution.hpp"

#include "Representation.h"

using namespace optframe;

// int (?)
typedef OPTFRAME_DEFAULT_ADS MY_ADS;

typedef CopySolution<RepKP, MY_ADS> SolutionKP;

#endif /*KP_SOLUTION_H_*/

