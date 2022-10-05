#ifndef EFP_SOLUTION_H_
#define EFP_SOLUTION_H_

#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "Evaluation.h"
#include "Representation.h"

using namespace optframe;

namespace HFM {
typedef Solution<RepHFM, OPTFRAME_DEFAULT_ADS> SolutionEFP;

typedef SolutionEFP SolutionHFM;

//typedef pair<SolutionEFP, EvaluationEFP> ESolutionEFP;

//typedef ESolutionEFP ESolutionHFM; // TODO: same naming

typedef pair<SolutionHFM, EvaluationHFM> ESolutionHFM;
typedef pair<SolutionHFM, MultiEvaluationHFM> EMSolutionHFM;

static_assert(XSolution<SolutionHFM>);
static_assert(XEvaluation<EvaluationHFM>);
static_assert(XESolution<ESolutionHFM>);
static_assert(XESolution<EMSolutionHFM>);
}  // namespace HFM

#endif /*EFP_SOLUTION_H_*/
