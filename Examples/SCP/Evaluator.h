#ifndef SCP_EVALUATOR_H_
#define SCP_EVALUATOR_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "../../Examples/SCP/DeltaStructure.h"
#include "../../Examples/SCP/Evaluation.h"
#include "../../Examples/SCP/ProblemInstance.h"
#include "../../Examples/SCP/Representation.h"
#include "../../Examples/SCP/Solution.h"

#define EPSILON_SCP 0.0001


namespace SCP
{

class SCPEvaluator: public Evaluator<RepSCP, MY_ADS >
{
private:
	ProblemInstance& pSCP;

	// Your private vars

public:
	SCPEvaluator(ProblemInstance& _pSCP);
	
	virtual ~SCPEvaluator();

	EvaluationSCP& evaluate(const RepSCP& rep);
	
	//EvaluationSCP& evaluate(const RepSCP& rep, const MY_ADS&);

	virtual bool betterThan(double f1, double f2);
	
	virtual bool isMinimization() const;
};

}

#endif /*SCP_EVALUATOR_H_*/

