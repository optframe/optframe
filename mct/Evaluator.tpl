#ifndef $project_EVALUATOR_HPP_
#define $project_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#define EPSILON_$project 0.0001

namespace $project
{

class $projectEvaluator: public Evaluator<Rep$project, OPTFRAME_DEFAULT_ADS $commadproject>
{
private:
	ProblemInstance& p$project;

	// Your private vars

public:
	$projectEvaluator(ProblemInstance& _p$project): // If necessary, add more parameters
	p$project(_p$project)
	{
		// Put the rest of your code here
	}
	
	virtual ~$projectEvaluator()
	{
	}

	Evaluation$project& evaluate(const Rep$project& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		// 'rep' is the representation of the solution

		double fo = 0; // Evaluation Function Value

		return * new Evaluation$project(fo $initializedelta);
	}

	virtual bool betterThan(const Evaluation< $deltaproject >& e1, const Evaluation< $deltaproject >& e2)
	{
		// $minmax
		return $epsilon
	}
	
    virtual bool isMinimization()
    {
        // $minmax
    	return " $minmax " == " MINIMIZATION ";
    }

};

}

#endif /*$project_EVALUATOR_HPP_*/

