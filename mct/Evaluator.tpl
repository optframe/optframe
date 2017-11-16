#ifndef $project_EVALUATOR_H_
#define $project_EVALUATOR_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "ProblemInstance.h"


namespace $project
{

// If you need ADS, you can inherit Evaluator<Rep$project, YourADS>
class MyEvaluator: public Evaluator<Rep$project>
{
private:
	ProblemInstance& p$project;

	// Your private vars

public:
	MyEvaluator(ProblemInstance& _p$project);
	
	virtual ~MyEvaluator();

    // you can replace OptFrameADS with your prefered ADS
	Evaluation evaluate(const Rep$project& rep, const OptFrameADS* ads);
	
	virtual bool isMinimization() const;
};

}

#endif /*$project_EVALUATOR_H_*/

