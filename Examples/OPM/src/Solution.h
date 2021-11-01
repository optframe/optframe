#ifndef OPM_SOLUTION_H_
#define OPM_SOLUTION_H_

#include "Representation.h"
#include <OptFrame/Solution.hpp>
#include <OptFrame/Solutions/CopySolution.hpp>

using namespace optframe;

namespace POLAD {
typedef CopySolution<RepOPM> SolutionOPM;
}
#endif /*OPM_SOLUTION_H_*/
