#ifndef KP_EVALUATOR_H_
#define KP_EVALUATOR_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Solution.h"

#include "ProblemInstance.h"

#define EPSILON_KP 0.0001


namespace KP
{

class MyEvaluator: public Evaluator<RepKP >
{
private:
	ProblemInstance& pKP;

	// Your private vars

public:
	MyEvaluator(ProblemInstance& _pKP);
	
	virtual ~MyEvaluator();

	Evaluation evaluate(const RepKP& rep, const OPTFRAME_DEFAULT_ADS*) override;
	
	//Evaluation& evaluate(const RepKP& rep, const MY_ADS&);
	
	virtual bool isMinimization() const override;
};

}

#endif /*KP_EVALUATOR_H_*/

