#include "Evaluator.h"

using namespace $project;

MyEvaluator::MyEvaluator(ProblemInstance& _p$project): // If necessary, add more parameters
p$project(_p$project)
{
	// Put the rest of your code here
}
	
MyEvaluator::~MyEvaluator()
{
}

Evaluation MyEvaluator::evaluate(const Rep$project& rep, const OptFrameADS* ads)
{
    // you can ignore parameter 'ads'
    // 'rep' is the representation of the solution

    double fo = 0; // Evaluation Function Value

    return Evaluation(fo);
}


bool MyEvaluator::isMinimization() const
{
	// $minmax
	return $isMin
}
