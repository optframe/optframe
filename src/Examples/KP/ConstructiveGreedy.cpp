#include "ConstructiveGreedy.h"

using namespace KP;

ConstructiveGreedy::ConstructiveGreedy(ProblemInstance& _pKP)
  : pKP(_pKP)
{
}

ConstructiveGreedy::~ConstructiveGreedy()
{
}

SolutionKP*
ConstructiveGreedy::generateSolution(double timelimit)
{
   RepKP newRep;

   return new SolutionKP(newRep);
}
