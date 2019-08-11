#ifndef OPM_SOLUTION_H_
#define OPM_SOLUTION_H_

#include "../../OptFrame/Solution.hpp"
#include "../../OptFrame/Solutions/CopySolution.hpp"
#include "Representation.h"

using namespace optframe;

namespace POLAD
{
typedef CopySolution<RepOPM> SolutionOPM;
}
#endif /*OPM_SOLUTION_H_*/

