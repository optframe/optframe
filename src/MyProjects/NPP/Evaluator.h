#ifndef NPP_EVALUATOR_H_
#define NPP_EVALUATOR_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "ProblemInstance.h"


namespace NPP
{

// If you need ADS, you can inherit Evaluator<RepNPP, YourADS>
class MyEvaluator: public Evaluator<RepNPP>
{
private:
	ProblemInstance& pNPP;

	// Your private vars

public:
	MyEvaluator(ProblemInstance& _pNPP);
	
	virtual ~MyEvaluator();

    // you can replace OptFrameADS with your prefered ADS
	Evaluation evaluate(const RepNPP& rep, const OptFrameADS* ads);
	
	virtual bool isMinimization() const;
};

}

#endif /*NPP_EVALUATOR_H_*/

