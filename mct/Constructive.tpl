#ifndef $project_CONSTRUCTIVE_$constructive_H_
#define $project_CONSTRUCTIVE_$constructive_H_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

using namespace optframe;

namespace $project
{

class Constructive$constructive: public Constructive<Rep$project, MY_ADS>
{
private:
   ProblemInstance& p$project;

   // Your private vars

public:
	
	Constructive$constructive(ProblemInstance& _p$project);
	
	virtual ~Constructive$constructive();

	Solution<Rep$project, MY_ADS>& generateSolution();
};

}

#endif /*$project_CONTRUCTIVE_$constructive_H_*/
