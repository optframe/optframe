#ifndef MODM_SOLUTION_H_
#define MODM_SOLUTION_H_

#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "ADS.h"
#include "Evaluation.h"
#include "Representation.h"

using namespace optframe;
using namespace MODM;

//typedef int MY_ADS;

//typedef Solution<RepMODM, AdsMODM> SolutionMODM;
typedef CopySolution<RepMODM, AdsMODM> SolutionMODM;

typedef pair<SolutionMODM, EvaluationMODM> ESolutionMODM;

typedef pair<SolutionMODM, MultiEvaluationMODM> EMSolutionMODM;

static_assert(XESolution<ESolutionMODM>);

static_assert(XSearch<ESolutionMODM, ESolutionMODM>);

#endif /*MODM_SOLUTION_H_*/
