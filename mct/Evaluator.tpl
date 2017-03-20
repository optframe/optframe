#ifndef $project_EVALUATOR_H_
#define $project_EVALUATOR_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Solution.h"

#include "ProblemInstance.h"

#define EPSILON_$project 0.0001


namespace $project
{

class MyEvaluator: public Evaluator<Rep$project, MY_ADS >
{
private:
	ProblemInstance& p$project;

	// Your private vars

public:
	MyEvaluator(ProblemInstance& _p$project);
	
	virtual ~MyEvaluator();

	Evaluation& evaluate(const Rep$project& rep);
	
	//Evaluation& evaluate(const Rep$project& rep, const MY_ADS&);
	
	virtual bool isMinimization() const;
};

}

#endif /*$project_EVALUATOR_H_*/

