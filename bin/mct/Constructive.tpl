#pragma once

#ifndef $project_CONSTRUCTIVE_$constructive_H_
#define $project_CONSTRUCTIVE_$constructive_H_

#include "../../OptFrame/Constructive.hpp"

#include "ProblemInstance.h"

#include "Representation.h"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

using namespace optframe;

namespace $project
{

// If you need ADS, you can inherit Constructive<Rep$project, YourADS>
class Constructive$constructive: public Constructive<Rep$project>
{
private:
   ProblemInstance& p$project;

   // Your private vars

public:
	
	Constructive$constructive(ProblemInstance& _p$project);
	
	virtual ~Constructive$constructive();
	
	string id() const
    {
        return Move<Rep$project >::idComponent().append(":Constructive$constructive");
    }

	Solution$project* generateSolution(double timelimit);
};

}

#endif /*$project_CONTRUCTIVE_$constructive_H_*/
