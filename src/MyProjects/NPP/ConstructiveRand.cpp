#include "ConstructiveRand.h"

using namespace NPP;

ConstructiveRand::ConstructiveRand(ProblemInstance& _pNPP): 
        pNPP(_pNPP)
{
}
	
ConstructiveRand::~ConstructiveRand()
{
}

SolutionNPP* ConstructiveRand::generateSolution(double timelimit)
{
    RepNPP newRep;

    for (int i = 0; i < pNPP.getSize(); ++i) {
        newRep.push_back(i);
    }
    std::random_shuffle(newRep.begin(), newRep.end());
    // if no Solution can be built in timelimit, return nullptr.
    return new SolutionNPP(newRep);			
}
