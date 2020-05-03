#ifndef MODM_SOLUTION_H_
#define MODM_SOLUTION_H_

#include "../../OptFrame/Solution.hpp"
#include "../../OptFrame/Solutions/CopySolution.hpp"
#include "Representation.h"
#include "ADS.h"
#include "Evaluation.h"

using namespace optframe;
using namespace MODM;

//typedef int MY_ADS;

//typedef Solution<RepMODM, AdsMODM> SolutionMODM;
typedef CopySolution<RepMODM, AdsMODM> SolutionMODM;

typedef pair<SolutionMODM, EvaluationMODM> ESolutionMODM;

static_assert(XESolution<ESolutionMODM>);

static_assert(XSearch<ESolutionMODM, ESolutionMODM>);

#endif /*MODM_SOLUTION_H_*/


