#include "ConstructiveRandom.h"

using namespace KP;

ConstructiveRandom::ConstructiveRandom(ProblemInstance& _pKP)
  : pKP(_pKP)
{
}

ConstructiveRandom::~ConstructiveRandom()
{
}

Solution<RepKP, MY_ADS>*
ConstructiveRandom::generateSolution(double timelimit)
{
   srand(5);
   RepKP newRep(pKP.N);

   for (int i = 0; i < pKP.N; i++)
      newRep[i] = rand() % 2;

   return new Solution<RepKP, MY_ADS>(newRep);
}
