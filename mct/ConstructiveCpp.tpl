#include "Constructive$constructive.h"

using namespace $project;

Constructive$constructive::Constructive$constructive(ProblemInstance& _p$project): 
        p$project(_p$project)
{
}
	
Constructive$constructive::~Constructive$constructive()
{
}

Solution<Rep$project, MY_ADS>& Constructive$constructive::generateSolution()
{
    Rep$project newRep;

    return * new TestSolution<Rep$project, MY_ADS>(newRep);			
}
