#include "ConstructiveGreedy.h"

using namespace KP;

ConstructiveGreedy::ConstructiveGreedy(ProblemInstance& _pKP)
  : pKP(_pKP)
{
}

ConstructiveGreedy::~ConstructiveGreedy()
{
}

Solution<RepKP, MY_ADS>*
ConstructiveGreedy::generateSolution(double timelimit) override
{
   RepKP newRep;

   return new Solution<RepKP, MY_ADS>(newRep);
}
