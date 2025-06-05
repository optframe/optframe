#include "Constructive$constructive.h"

using namespace $project;

Constructive$constructive::Constructive$constructive(ProblemInstance& _p$project): 
        p$project(_p$project)
{
}
	
Constructive$constructive::~Constructive$constructive()
{
}

std::optional< Solution$project > Constructive$constructive::generateSolution(double timelimit)
{
    Rep$project newRep;

    // if no Solution can be built in timelimit, return nullptr.
    return make_optional(Solution$project(newRep));			
}
