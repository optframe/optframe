#ifndef NPP_CONSTRUCTIVE_Greedy_H_
#define NPP_CONSTRUCTIVE_Greedy_H_

#include "../../OptFrame/Constructive.hpp"

#include "ProblemInstance.h"

#include "Representation.h"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

using namespace optframe;

namespace NPP
{

// If you need ADS, you can inherit Constructive<RepNPP, YourADS>
class ConstructiveGreedy: public Constructive<RepNPP>
{
private:
   ProblemInstance& pNPP;

   // Your private vars

public:
	
	ConstructiveGreedy(ProblemInstance& _pNPP);
	
	virtual ~ConstructiveGreedy();

    string id() const
    {
        return Constructive<RepNPP>::idComponent().append(":ConstructiveGreedy");
    }

	SolutionNPP* generateSolution(double timelimit);
};

}

#endif /*NPP_CONTRUCTIVE_Greedy_H_*/
