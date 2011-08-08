#ifndef $project_INITIALSOLUTION_$initialsolution_HPP_
#define $project_INITIALSOLUTION_$initialsolution_HPP_

#include "../../OptFrame/InitialSolution.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

class $projectInitialSolution$initialsolution: public InitialSolution<Rep$project>
{
private:
   $projectProblemInstance& p$project;

   // Your private vars

public:
	
	$projectInitialSolution$initialsolution($projectProblemInstance& _p$project): // If necessary, add more parameters
        p$project(_p$project)
	{
	}

	Solution<Rep$project>& generateSolution()
	{
	   Rep$project newRep;

	   return * new TestSolution<Rep$project>(newRep);			
	}
	
};

#endif /*$project_INITIALSOLUTION_$initialsolution_HPP_*/
