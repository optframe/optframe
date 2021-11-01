#ifndef PN_SOLUTION_H_
#define PN_SOLUTION_H_

#include "Representation.h"
#include <OptFrame/Solution.hpp>
#include <OptFrame/Solutions/CopySolution.hpp>

using namespace optframe;

typedef int MY_ADS;

typedef CopySolution<RepPN, MY_ADS> SolutionPN;

#endif /*PN_SOLUTION_H_*/
