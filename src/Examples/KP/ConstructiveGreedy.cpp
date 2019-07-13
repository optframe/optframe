#include "ConstructiveGreedy.h"

using namespace KP;

ConstructiveGreedy::ConstructiveGreedy(ProblemInstance& _pKP): 
        pKP(_pKP)
{
}
	
ConstructiveGreedy::~ConstructiveGreedy()
{
}

Solution<RepKP, MY_ADS>& ConstructiveGreedy::generateSolution()
{
    RepKP newRep;

    return * new TestSolution<RepKP, MY_ADS>(newRep);			
}
