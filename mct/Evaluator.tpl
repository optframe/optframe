#ifndef $project_EVALUATOR_H_
#define $project_EVALUATOR_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#define EPSILON_$project 0.0001


namespace $project
{

class $projectEvaluator: public Evaluator<Rep$project, MY_ADS >
{
private:
	ProblemInstance& p$project;

	// Your private vars

public:
	$projectEvaluator(ProblemInstance& _p$project);
	
	virtual ~$projectEvaluator();

	Evaluation$project& evaluate(const Rep$project& rep);
	
	//Evaluation$project& evaluate(const Rep$project& rep, const MY_ADS&);

	virtual bool betterThan(double f1, double f2);
	
	virtual bool isMinimization() const;
};

}

#endif /*$project_EVALUATOR_H_*/

