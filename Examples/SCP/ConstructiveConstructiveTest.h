#ifndef SCP_CONSTRUCTIVE_ConstructiveTest_H_
#define SCP_CONSTRUCTIVE_ConstructiveTest_H_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>
#include "../../Examples/SCP/Evaluator.h"
#include "../../Examples/SCP/ProblemInstance.h"
#include "../../Examples/SCP/Representation.h"
#include "../../Examples/SCP/Solution.h"

using namespace std;

using namespace optframe;

namespace SCP
{

class ConstructiveConstructiveTest: public Constructive<RepSCP, MY_ADS>
{
private:
   ProblemInstance& pSCP;

   // Your private vars

public:
	
	ConstructiveConstructiveTest(ProblemInstance& _pSCP);
	
	virtual ~ConstructiveConstructiveTest();

	Solution<RepSCP, MY_ADS>& generateSolution();
};

}

#endif /*SCP_CONTRUCTIVE_ConstructiveTest_H_*/
