#ifndef KP_CONSTRUCTIVE_Random_H_
#define KP_CONSTRUCTIVE_Random_H_

#include <OptFrame/Constructive.hpp>
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

//class ConstructiveRandom: public Constructive<SolutionKP>
class ConstructiveRandom: public InitialSearch<ESolutionKP, EvaluationKP>
{
private:
   ProblemInstance& pKP;
   GeneralEvaluator<ESolutionKP>& eval;

public:
	
	ConstructiveRandom(ProblemInstance& _pKP, GeneralEvaluator<ESolutionKP>& _eval);
	
	virtual ~ConstructiveRandom();

	//std::optional<SolutionKP> generateSolution(double timelimit) override;
   std::optional<ESolutionKP> initialSearch(const StopCriteria<EvaluationKP>& stop) override;
};

}

#endif /*KP_CONTRUCTIVE_Random_H_*/
