#ifndef MODM_SOLUTION_H_
#define MODM_SOLUTION_H_

#include "../../OptFrame/Solution.hpp"
#include "Representation.h"
#include "ADS.h"

using namespace optframe;
using namespace MODM;

//typedef int MY_ADS;

typedef Solution<RepMODM, AdsMODM> SolutionMODM;

#endif /*MODM_SOLUTION_H_*/


