#include "ConstructiveRandom.h"

using namespace KP;

ConstructiveRandom::ConstructiveRandom(ProblemInstance& _pKP, GeneralEvaluator<ESolutionKP>& _eval)
  : pKP(_pKP), eval(_eval)
{
}

ConstructiveRandom::~ConstructiveRandom()
{
}

//std::optional<SolutionKP>
//ConstructiveRandom::generateSolution(double timelimit)
std::pair<std::optional<ESolutionKP>, SearchStatus>
ConstructiveRandom::initialSearch(const StopCriteria<EvaluationKP>& sosc)
{
   //double timelimit = sosc.timelimit;
   srand(5);
   RepKP newRep(pKP.N);

   for (int i = 0; i < pKP.N; i++)
      newRep[i] = rand() % 2;

   ESolutionKP se = ESolutionKP(newRep, EvaluationKP());
   eval.reevaluate(se);
   return make_pair(make_optional(se), SearchStatus::NO_REPORT);
}
