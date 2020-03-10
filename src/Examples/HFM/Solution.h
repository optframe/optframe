#ifndef EFP_SOLUTION_H_
#define EFP_SOLUTION_H_

#include "../../OptFrame/Solution.hpp"
#include "../../OptFrame/Solutions/CopySolution.hpp"
#include "Representation.h"
#include "Evaluation.h"

using namespace optframe;

namespace HFM
{
typedef Solution<RepHFM,OPTFRAME_DEFAULT_ADS> SolutionEFP;

typedef SolutionEFP SolutionHFM;

//typedef pair<SolutionEFP, EvaluationEFP> ESolutionEFP;

//typedef ESolutionEFP ESolutionHFM; // TODO: same naming

typedef pair<SolutionHFM, EvaluationHFM> ESolutionHFM;
typedef pair<SolutionHFM, MultiEvaluationHFM> EMSolutionHFM;

static_assert(XSolution<SolutionHFM>);
static_assert(XEvaluation<EvaluationHFM>);
static_assert(XESolution<ESolutionHFM>);
static_assert(XESolution<EMSolutionHFM>);
}

#endif /*EFP_SOLUTION_H_*/

