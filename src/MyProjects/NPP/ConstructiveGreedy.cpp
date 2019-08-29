#include "ConstructiveGreedy.h"

using namespace NPP;

ConstructiveGreedy::ConstructiveGreedy(ProblemInstance& _pNPP):
        pNPP(_pNPP)
{
}

ConstructiveGreedy::~ConstructiveGreedy()
{
}

SolutionNPP* ConstructiveGreedy::generateSolution(double timelimit)
{
    RepNPP newRep;

    for (int i = 0; i < pNPP.getSize(); ++i) {
        newRep.push_back(i);
    }
    std::random_shuffle(newRep.begin(), newRep.end());
    // if no Solution can be built in timelimit, return nullptr.
    return new SolutionNPP(newRep);
}
