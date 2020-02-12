#include "ConstructiveGreedy.h"

using namespace KP;

ConstructiveGreedy::ConstructiveGreedy(ProblemInstance& _pKP)
  : pKP(_pKP)
{
}

ConstructiveGreedy::~ConstructiveGreedy()
{
}

std::optional<SolutionKP>
ConstructiveGreedy::generateSolution(double timelimit)
{
   RepKP newRep;

   return make_optional(SolutionKP(newRep));
}
