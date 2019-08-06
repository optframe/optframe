#ifndef KP_CONSTRUCTIVE_Greedy_H_
#define KP_CONSTRUCTIVE_Greedy_H_

#include "../../OptFrame/Constructive.hpp"
//#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

using namespace optframe;

namespace KP
{

class ConstructiveGreedy: public Constructive<RepKP, MY_ADS>
{
private:
   ProblemInstance& pKP;

   // Your private vars

public:
	
	ConstructiveGreedy(ProblemInstance& _pKP);
	
	virtual ~ConstructiveGreedy();

	Solution<RepKP, MY_ADS>* generateSolution(double timelimit) override;
};

}

#endif /*KP_CONTRUCTIVE_Greedy_H_*/
