#include "ConstructiveRandom.h"

using namespace KP;

ConstructiveRandom::ConstructiveRandom(ProblemInstance& _pKP)
  : pKP(_pKP)
{
}

ConstructiveRandom::~ConstructiveRandom()
{
}

std::optional<SolutionKP>
ConstructiveRandom::generateSolution(double timelimit)
{
   srand(5);
   RepKP newRep(pKP.N);

   for (int i = 0; i < pKP.N; i++)
      newRep[i] = rand() % 2;

   return make_optional(SolutionKP(newRep));
}
