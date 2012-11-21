#ifndef $project_CONSTRUCTIVE_$constructive_HPP_
#define $project_CONSTRUCTIVE_$constructive_HPP_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

namespace $project
{

class Constructive$constructive: public Constructive<Rep$project>
{
private:
   ProblemInstance& p$project;

   // Your private vars

public:
	
	Constructive$constructive(ProblemInstance& _p$project): // If necessary, add more parameters
        p$project(_p$project)
	{
	}
	
	virtual ~Constructive$constructive()
	{
	}

	Solution<Rep$project>& generateSolution()
	{
	   Rep$project newRep;

	   return * new TestSolution<Rep$project>(newRep);			
	}
	
};

}

#endif /*$project_CONTRUCTIVE_$constructive_HPP_*/
